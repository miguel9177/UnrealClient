// Fill out your copyright notice in the Description page of Project Settings.


#include "NetManager.h"
#include <string>
#include "GameManager.h"

TArray<UNetworkGameObject*> ANetManager::localNetObjects;
ANetManager* ANetManager::singleton; //declare the pointer
float ANetManager::timePastSinceBeginPlay = 0;


// Sets default values
ANetManager::ANetManager()
{
	PrimaryActorTick.bCanEverTick = true;
	Socket = nullptr;
}

ANetManager::~ANetManager()
{

}

void ANetManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (singleton == nullptr) singleton = this; //if it’s null, it becomes the current instance

	UE_LOG(LogTemp, Warning, TEXT("Hello"));
	SocketSubsystem = nullptr;
	//more macro code. We’re using Unreal’s low level generic networking (as opposed to it’s higher level game-oriented solution).
	if (SocketSubsystem == nullptr)	SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	//packet and buffer sizes
	SendSize = 2 * 1024 * 1024;
	BufferSize = 2 * 1024 * 1024;

	//local endpoint
	LocalEndpoint = FIPv4Endpoint(FIPv4Address::Any, LocalPort);

	FIPv4Address::Parse(IP, RemoteAddress);
	//server endpoint
	RemoteEndpoint = FIPv4Endpoint(RemoteAddress, RemotePort);

	Socket = nullptr;

	if (SocketSubsystem != nullptr)
	{
		if (Socket == nullptr)
		{
			//similar to C#, we use an API to build the socket based on configuration parameters
			Socket = FUdpSocketBuilder(SocketDescription)
				.AsNonBlocking()
				.AsReusable()
				.BoundToEndpoint(LocalEndpoint)
				.WithSendBufferSize(SendSize)
				.WithReceiveBufferSize(BufferSize)
				.WithBroadcast();
		}
	}


	//	Listen(); Uncomment this line when you have added the Listen(); method.

	FString t = "FirstEntrance";
	sendMessage(t); // Send Message Test


}

// Called when the game starts or when spawned
void ANetManager::BeginPlay()
{
	Super::BeginPlay();
}

void ANetManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ANetManager::localNetObjects.Empty();
	SocketSubsystem->DestroySocket(Socket);
	Socket = nullptr;
	SocketSubsystem = nullptr;
	singleton = nullptr; //it becomes null for next time we play (otherwise it’ll point to the previously destroyed version from the last session
}

// Called every frame
void ANetManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Super::Tick(DeltaTime);
	timePastSinceBeginPlay += DeltaTime;

	for (UNetworkGameObject* netObject : ANetManager::localNetObjects) 
	{
		float timePassedSinceRequestingId = netObject->requestedIdInfo.timeIdWasRequested - timePastSinceBeginPlay;
		if (netObject->GetGlobalID() == 0 && (netObject->requestedIdInfo.requestedId == false || timePassedSinceRequestingId < -5))
		{
			FString t = "I need a UID for local object:" + FString::FromInt(netObject->GetLocalID());
			sendMessage(t);
			netObject->requestedIdInfo.requestedId = true;
			netObject->requestedIdInfo.timeIdWasRequested = timePastSinceBeginPlay;
		}
		if (netObject->GetIsLocallyOwned() && netObject->GetGlobalID() != 0) 
		{
			UE_LOG(LogTemp, Warning, TEXT("Sending: %s"), *netObject->ToPacket());
			sendMessage(netObject->ToPacket());
		}
	}

	Listen(); // Listen for messages

	//FString t = "I'm an Unreal client!";
	//sendMessage(t); // Send Message Test


}

void ANetManager::Listen()
{
	TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	uint32 Size;
	//TODO: FIX INFINITE LOOP BUG
	while (Socket->HasPendingData(Size))
	{
		uint8* Recv = new uint8[Size];
		int32 BytesRead = 0;

		ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));
		Socket->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), BytesRead, *targetAddr);

		char ansiiData[4096];
		memcpy(ansiiData, ReceivedData.GetData(), BytesRead);
		ansiiData[BytesRead] = 0;

		FString data = ANSI_TO_TCHAR(ansiiData);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Message by UDP: " + data);

		if (data.Contains("Assigned UID:")) 
		{

			FString message, info;
			//split off the 'Assigned UID:' bit, by delimiting at the :
			if (data.Split(TEXT(":"), &message, &info)) 
			{
				FString lid, gid;
				//split into local and global ID, by delimiting at the ;
				if (info.Split(TEXT(";"), &lid, &gid)) 
				{
					//the Atoi function is the equivalent of Int32.Parse in C#, converting a string to an int32
					int32 intGlobalID = FCString::Atoi(*gid);
					int32 intLocalID = FCString::Atoi(*lid);

					//iterate netObjects, find the one the local ID corresponds to, and assign the global ID
					for (UNetworkGameObject* netObject : ANetManager::localNetObjects) 
					{
						if (netObject->GetLocalID() == intLocalID) 
						{
							netObject->SetGlobalID(intGlobalID);
							UE_LOG(LogTemp, Warning, TEXT("Assigned: %d"), intGlobalID);
						}

					}

				}
			}

		}
		else if (data.Contains("Object data;")) 
		{
			UE_LOG(LogTemp, Warning, TEXT("parsing state data"));
			bool foundActor = false;
			for (UNetworkGameObject* netObject : ANetManager::localNetObjects) {
				if (netObject->GetGlobalID() == netObject->GlobalIDFromPacket(data)) {
					if (!netObject->GetIsLocallyOwned()) {
						netObject->FromPacket(data);
					}
					foundActor = true;
				}
			}

			if (!foundActor) {
				UE_LOG(LogTemp, Warning, TEXT("spawning"));
				AActor* actor = GetWorld()->SpawnActor<AActor>(OtherPlayerAvatars, FVector::ZeroVector, FRotator::ZeroRotator);
				UNetworkGameObject* netActor = actor->FindComponentByClass<UNetworkGameObject>();
				netActor->SetGlobalID(netActor->GlobalIDFromPacket(data));
				netActor->FromPacket(data);
			}

		}


	}
	//while (Socket->HasPendingData(Size))
		//{
	//}
	UE_LOG(LogTemp, Warning, TEXT("Hello"));
}

bool ANetManager::sendMessage(FString Message)
{
	if (!Socket) return false;
	int32 BytesSent;

	FTimespan waitTime = FTimespan(10);
	//this is where we create the packet, in this case by serialising the character array
	TCHAR* serializedChar = Message.GetCharArray().GetData();
	int32 size = FCString::Strlen(serializedChar);

	bool success = Socket->SendTo((uint8*)TCHAR_TO_UTF8(serializedChar), size, BytesSent, *RemoteEndpoint.ToInternetAddr());
	UE_LOG(LogTemp, Warning, TEXT("Sent message: %s : %s : Address - %s : BytesSent - %d"), *Message, (success ? TEXT("true") : TEXT("false")), *RemoteEndpoint.ToString(), BytesSent);
	//UE_LOG lets us log error messages, not dissimilar to Debug.Log

	if (success && BytesSent > 0) return true;
	else return false;

}

void ANetManager::messageQueue()
{
}

void ANetManager::AddNetObject(UNetworkGameObject* component) {
	ANetManager::localNetObjects.Add(component);
	if ((component->GetGlobalID() == 0 && (component->requestedIdInfo.requestedId == false || -5 < (component->requestedIdInfo.timeIdWasRequested - timePastSinceBeginPlay))) && (component->GetIsLocallyOwned())) {
		FString t = "I need a UID for local object:" + FString::FromInt(component->GetLocalID());
		sendMessage(t);
		component->requestedIdInfo.requestedId = true;
	}
}

#pragma region Network Events

//this is called when we shot a player with our gun
void ANetManager::AnEnemyPlayerWasShotByUs(UNetworkGameObject* characterWeHit, FString nameOfWeapon)
{
	FString enemyPlayerHitMessage = "GameplayEvent: Player shot another player: Player with id;" + FString::FromInt(AGameManager::GetInstance()->GetOurPlayerNetworkGameObject()->GetGlobalID()) + "; shot player with id; " + FString::FromInt(characterWeHit->GetGlobalID()) + "; with weapon;" + nameOfWeapon;
	sendMessage(enemyPlayerHitMessage);
}

#pragma endregion


