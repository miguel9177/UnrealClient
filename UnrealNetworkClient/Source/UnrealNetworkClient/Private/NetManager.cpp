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

	//UE_LOG(LogTemp, Warning, TEXT("Hello"));
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

	//if socket is subsystem is not null
	if (SocketSubsystem != nullptr)
	{
		//if the socket is null
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
	//since we ended the playe we restart the variables and destroy the socket
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
	//we increase the begin player timer
	timePastSinceBeginPlay += DeltaTime;
	//we increase the time since send message timer, to be able to controll how many messages are sent by second
	timePastSinceSend += DeltaTime;

	//we loop through all local objects
	for (UNetworkGameObject* netObject : ANetManager::localNetObjects) 
	{
		//we get the time passed since we last requested the id
		float timePassedSinceRequestingId = netObject->requestedIdInfo.timeIdWasRequested - timePastSinceBeginPlay;
		//if the globl id is 0 and if we havent requested id yet or the time past since we requested is more then 5 seconds we request the global id 
		if (netObject->GetGlobalID() == 0 && (netObject->requestedIdInfo.requestedId == false || timePassedSinceRequestingId < -5))
		{
			//we create a message for request uid of the local object
			FString t = "I need a UID for local object:" + FString::FromInt(netObject->GetLocalID());
			//we send the message
			sendMessage(t);
			//we tell the code that we already requested the id
			netObject->requestedIdInfo.requestedId = true;
			//we store the time that the id was requested
			netObject->requestedIdInfo.timeIdWasRequested = timePastSinceBeginPlay;
		}
		//if the object is locally owned and the global id is diferent then 0
		if (netObject->GetIsLocallyOwned() && netObject->GetGlobalID() != 0) 
		{
			//if the time past since we sent was bigger then 0.5 seconds we send a new packet
			if (timePastSinceSend > 0.5f)
			{
				UE_LOG(LogTemp, Warning, TEXT("Sending: %s"), *netObject->ToPacket());
				//this sends the message
				sendMessage(netObject->ToPacket());
				//this makes the time past to 0
				timePastSinceSend = 0;
			}
		}
	}

	Listen(); // Listen for messages

	//FString t = "I'm an Unreal client!";
	//sendMessage(t); // Send Message Test


}

//this listens to the messages
void ANetManager::Listen()
{
	TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	uint32 Size;
	//this loops until theres no messages left
	while (Socket->HasPendingData(Size))
	{
		uint8* Recv = new uint8[Size];
		int32 BytesRead = 0;
		//this receives the data from the socket
		ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));
		Socket->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), BytesRead, *targetAddr);

		//we memcopy the bytes received and store it on the asii data
		char ansiiData[4096];
		memcpy(ansiiData, ReceivedData.GetData(), BytesRead);
		ansiiData[BytesRead] = 0;

		//this converts the data received to a string
		FString data = ANSI_TO_TCHAR(ansiiData);

		//we write the message received on the gameengine
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Message by UDP: " + data);

		//if the message has Assigned UID 
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
						//if the local id is the same as the one received, we update its global id
						if (netObject->GetLocalID() == intLocalID) 
						{
							netObject->SetGlobalID(intGlobalID);
							UE_LOG(LogTemp, Warning, TEXT("Assigned: %d"), intGlobalID);
						}

					}

				}
			}

		}
		//if the message is an object data
		else if (data.Contains("Object data;")) 
		{
			//we write on the log that we are parsing the data
			UE_LOG(LogTemp, Warning, TEXT("parsing state data"));
			//this will be true if we did find the actor
			bool foundActor = false;
			//we loop through all net objects
			for (UNetworkGameObject* netObject : ANetManager::localNetObjects) {
				//if the global id is the same as the global id received on the message
				if (netObject->GetGlobalID() == netObject->GlobalIDFromPacket(data)) {
					//if the objecct is not locally owned we update its pos and rot
					if (!netObject->GetIsLocallyOwned()) {
						netObject->FromPacket(data);
					}
					//if the object is locally owned we update the hp
					else
					{
						netObject->UpdateHpFromPacket(data);
					}
					//we tell the code that we found an actor
					foundActor = true;
				}
			}

			//if we didnt find an actor
			if (!foundActor) {
				//we tell the log that we are spawning a actor
				UE_LOG(LogTemp, Warning, TEXT("spawning"));
				//we spawn a new actor
				AActor* actor = GetWorld()->SpawnActor<AActor>(OtherPlayerAvatars, FVector::ZeroVector, FRotator::ZeroRotator);
				//we get the network gameobject of the spawned actor
				UNetworkGameObject* netActor = actor->FindComponentByClass<UNetworkGameObject>();
				//we set its global id
				netActor->SetGlobalID(netActor->GlobalIDFromPacket(data));
				//we update its pos and rot
				netActor->FromPacket(data);
			}

		}


	}
	//while (Socket->HasPendingData(Size))
		//{
	//}
	UE_LOG(LogTemp, Warning, TEXT("Hello"));
}
//this sends messages to the server
bool ANetManager::sendMessage(FString Message)
{
	//if the socket is null we return
	if (!Socket) return false;
	int32 BytesSent;//this will store the number of bytes to send

	FTimespan waitTime = FTimespan(10);
	//twe convert the message into an a tchar
	TCHAR* serializedChar = Message.GetCharArray().GetData();
	int32 size = FCString::Strlen(serializedChar);
	//we sendd the message
	bool success = Socket->SendTo((uint8*)TCHAR_TO_UTF8(serializedChar), size, BytesSent, *RemoteEndpoint.ToInternetAddr());
	UE_LOG(LogTemp, Warning, TEXT("Sent message: %s : %s : Address - %s : BytesSent - %d"), *Message, (success ? TEXT("true") : TEXT("false")), *RemoteEndpoint.ToString(), BytesSent);
	//UE_LOG lets us log error messages, not dissimilar to Debug.Log

	if (success && BytesSent > 0) return true;
	else return false;

}

void ANetManager::messageQueue()
{
}

//this adds a new object
void ANetManager::AddNetObject(UNetworkGameObject* component) {
	//we add a new component to the net objects
	ANetManager::localNetObjects.Add(component);
	//if the new network gameobject global id is 0 and hasnt requestedd id or the time since requested is biger then 5 and the component is locally owned, we request a new uid for this object
	if ((component->GetGlobalID() == 0 && (component->requestedIdInfo.requestedId == false || -5 < (component->requestedIdInfo.timeIdWasRequested - timePastSinceBeginPlay))) && (component->GetIsLocallyOwned())) {
		FString t = "I need a UID for local object:" + FString::FromInt(component->GetLocalID());//we create the string with the uid request message
		sendMessage(t);//we send the message
		component->requestedIdInfo.requestedId = true;//we tell the code tha this object has already requested an id
	}
}

#pragma region Network Events

//this is called when we shot a player with our gun
void ANetManager::AnEnemyPlayerWasShotByUs(UNetworkGameObject* characterWeHit, FString nameOfWeapon)
{
	//this creates the string with the player shot another message
	FString enemyPlayerHitMessage = "GameplayEvent: Player shot another player: Player with id;" + FString::FromInt(AGameManager::GetInstance()->GetOurPlayerNetworkGameObject()->GetGlobalID()) + "; shot player with id; " + FString::FromInt(characterWeHit->GetGlobalID()) + "; with weapon;" + nameOfWeapon;
	sendMessage(enemyPlayerHitMessage);//we send the message
}

#pragma endregion


