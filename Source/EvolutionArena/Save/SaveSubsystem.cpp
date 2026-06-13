// Copyright Evolution Arena. All Rights Reserved.

#include "Save/SaveSubsystem.h"
#include "Save/EvoSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogEvoSave, Log, All);

void USaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	EnsureSaveObject();
	if (HasSaveOnDisk())
	{
		if (!LoadFromDisk())
		{
			UE_LOG(LogEvoSave, Warning, TEXT("Existing save failed to load; starting a new game."));
			NewGame();
		}
	}
	else
	{
		NewGame();
	}
}

void USaveSubsystem::EnsureSaveObject()
{
	if (!SaveGameObject)
	{
		SaveGameObject = Cast<UEvoSaveGame>(UGameplayStatics::CreateSaveGameObject(UEvoSaveGame::StaticClass()));
	}
}

FPlayerSave& USaveSubsystem::GetPlayerData()
{
	EnsureSaveObject();
	return SaveGameObject->Data;
}

void USaveSubsystem::NewGame()
{
	EnsureSaveObject();
	SaveGameObject->Data = FPlayerSave();
	SaveGameObject->Data.SaveVersion = CurrentSaveVersion;
}

bool USaveSubsystem::HasSaveOnDisk() const
{
	return IFileManager::Get().FileExists(*GetSaveFilePath());
}

FString USaveSubsystem::GetSaveFilePath()
{
	return FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SaveGames"), TEXT("Player.sav"));
}

bool USaveSubsystem::SaveToDisk()
{
	EnsureSaveObject();
	SaveGameObject->Data.SaveVersion = CurrentSaveVersion;
	SaveGameObject->Data.LastSavedUtc = FDateTime::UtcNow();

	TArray<uint8> Bytes;
	if (!UGameplayStatics::SaveGameToMemory(SaveGameObject, Bytes))
	{
		UE_LOG(LogEvoSave, Error, TEXT("SaveToDisk: SaveGameToMemory failed."));
		return false;
	}

	if (!AtomicWriteFile(GetSaveFilePath(), Bytes))
	{
		UE_LOG(LogEvoSave, Error, TEXT("SaveToDisk: atomic write failed for %s."), *GetSaveFilePath());
		return false;
	}

	return true;
}

bool USaveSubsystem::LoadFromDisk()
{
	TArray<uint8> Bytes;
	if (!FFileHelper::LoadFileToArray(Bytes, *GetSaveFilePath()))
	{
		return false;
	}

	UEvoSaveGame* Loaded = Cast<UEvoSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));
	if (!Loaded)
	{
		UE_LOG(LogEvoSave, Error, TEXT("LoadFromDisk: save data missing or corrupt."));
		return false;
	}

	MigrateIfNeeded(Loaded->Data);

	EnsureSaveObject();
	SaveGameObject->Data = Loaded->Data;
	return true;
}

bool USaveSubsystem::AtomicWriteFile(const FString& FilePath, const TArray<uint8>& Bytes)
{
	// Write to a sibling temp file first; the rename below is the atomic commit, so
	// a crash mid-write can never corrupt the existing save.
	const FString TempPath = FilePath + TEXT(".tmp");
	if (!FFileHelper::SaveArrayToFile(Bytes, *TempPath))
	{
		return false;
	}

	if (!IFileManager::Get().Move(*FilePath, *TempPath, /*bReplace=*/ true))
	{
		IFileManager::Get().Delete(*TempPath, /*RequireExists=*/ false, /*EvenReadOnly=*/ true, /*Quiet=*/ true);
		return false;
	}

	return true;
}

void USaveSubsystem::MigrateIfNeeded(FPlayerSave& Data)
{
	// Upgrade older saves step by step. Only v1 exists today, so this is a no-op
	// framework: when the format breaks, add ordered `if (Data.SaveVersion < N)`
	// blocks here before the version is stamped current.
	if (Data.SaveVersion < CurrentSaveVersion)
	{
		// (future ordered migration steps go here)
		Data.SaveVersion = CurrentSaveVersion;
	}
}
