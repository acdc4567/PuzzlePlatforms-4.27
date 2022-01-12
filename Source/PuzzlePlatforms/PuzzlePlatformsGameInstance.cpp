// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzlePlatformsGameInstance.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

#include "OnlineSessionSettings.h"


#include "PlatformTrigger.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/MenuWidget.h"

const static FName Ssn_Name=TEXT("MySessionGame");

UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance(const FObjectInitializer & ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if (!ensure(MenuBPClass.Class != nullptr)) return;

	MenuClass = MenuBPClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/MenuSystem/WBP_InGameMenu"));
	if (!ensure(InGameMenuBPClass.Class != nullptr)) return;

	InGameMenuClass = InGameMenuBPClass.Class;
}

void UPuzzlePlatformsGameInstance::Init()
{
	IOnlineSubsystem* Subsystem=IOnlineSubsystem::Get();
	if(Subsystem!=nullptr){
		UE_LOG(LogTemp, Warning, TEXT("Found subsystem %s"), *Subsystem->GetSubsystemName().ToString());	
		SessionInterface=Subsystem->GetSessionInterface();
		if(SessionInterface.IsValid()){
			
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this,&UPuzzlePlatformsGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this,&UPuzzlePlatformsGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this,&UPuzzlePlatformsGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this,&UPuzzlePlatformsGameInstance::OnJoinSessionComplete);
			
			
		}
	}
	else{
		UE_LOG(LogTemp, Warning, TEXT("Found no subsystem "));	
		
	}
	
}

void UPuzzlePlatformsGameInstance::LoadMenuWidget()
{
	if (!ensure(MenuClass != nullptr)) return;

	Menux = CreateWidget<UMainMenu>(this, MenuClass);
	if (!ensure(Menux != nullptr)) return;

	Menux->Setup();

	Menux->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::InGameLoadMenu()
{
	if (!ensure(InGameMenuClass != nullptr)) return;

	UMenuWidget* Menux1 = CreateWidget<UMenuWidget>(this, InGameMenuClass);
	if (!ensure(Menux1 != nullptr)) return;

	Menux1->Setup();

	Menux1->SetMenuInterface(this);
}


void UPuzzlePlatformsGameInstance::Host()
{
	if(SessionInterface.IsValid()){
		auto ExistingSession= SessionInterface->GetNamedSession(Ssn_Name);
		if(ExistingSession!=nullptr){
			SessionInterface->DestroySession(Ssn_Name);
		}
		else{
			CreateSession();
		}
		
	}
	
}

void UPuzzlePlatformsGameInstance::CreateSession(){
	if(SessionInterface.IsValid()){
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bIsLANMatch=false;
		SessionSettings.NumPublicConnections=5;
		SessionSettings.bShouldAdvertise=true;
		SessionSettings.bUseLobbiesIfAvailable=true;
		SessionSettings.bUsesPresence=true;
		SessionInterface->CreateSession(0, Ssn_Name, SessionSettings);
	}
	


}

void UPuzzlePlatformsGameInstance::OnCreateSessionComplete(FName SessionName,bool Success){
	if(!Success){
		UE_LOG(LogTemp, Warning, TEXT("CouldNotCreateSession "));	
		return;
	}
	if (Menux != nullptr)
	{
		Menux->Teardown();
	}

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	World->ServerTravel("/Game/ThirdPersonCPP/Maps/DefaultMap?listen");
}


void UPuzzlePlatformsGameInstance::OnDestroySessionComplete(FName SessionName,bool Success){
	if (Success)
	{
		CreateSession();
	}
}

void UPuzzlePlatformsGameInstance::RefreshServerList(){
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid()){
		//SessionSearch->bIsLanQuery = true;
		SessionSearch->MaxSearchResults=20;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE,true,EOnlineComparisonOp::Equals);
		UE_LOG(LogTemp, Warning, TEXT("StartingToFindSessions"));
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UPuzzlePlatformsGameInstance::OnFindSessionsComplete(bool Success){

	UE_LOG(LogTemp, Warning, TEXT("FinishedFindingSessions "));
	if(Success&&SessionSearch.IsValid()&&Menux!=nullptr){
		TArray<FString> ServerNames;
		UE_LOG(LogTemp, Warning, TEXT("FinishedFoundSessions "));
		for(const FOnlineSessionSearchResult& SearchResult:SessionSearch->SearchResults){
			UE_LOG(LogTemp, Warning, TEXT("Found Sessions: %s "), *SearchResult.GetSessionIdStr());
			ServerNames.Add(SearchResult.GetSessionIdStr());
		}
		Menux->SetServerList(ServerNames);
	}
}




void UPuzzlePlatformsGameInstance::Join(uint32 Index)
{
	if(!SessionInterface.IsValid()) return;
	if(!SessionSearch.IsValid()) return;
	if (Menux != nullptr)
	{
		//Menu->SetServerList({"Test1","Test2"});
		Menux->Teardown();
	}
	SessionInterface->JoinSession(0,Ssn_Name,SessionSearch->SearchResults[Index]);

	/*
	

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	*/
}

void UPuzzlePlatformsGameInstance::OnJoinSessionComplete(FName SessionName,EOnJoinSessionCompleteResult::Type Result){
	if (!SessionInterface.IsValid())
		return;
	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("CouldnotGetConnectString "));
		return;
	}
	UEngine *Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UPuzzlePlatformsGameInstance::LoadMainMenu()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel("/Game/MenuSystem/MainMenu", ETravelType::TRAVEL_Absolute);
}



//UPuzzlePlatformsGameInstance
