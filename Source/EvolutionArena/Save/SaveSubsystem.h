// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Save/PlayerSave.h"
#include "SaveSubsystem.generated.h"

class UEvoSaveGame;

/**
 * The single authority for player persistence. The local file is the source of
 * truth (EOS cloud sync layers on later, per docs/TechSpec.md §7). All player
 * data goes through here — never serialize saves ad hoc (docs/rules.md).
 *
 * The core pieces (serialize, atomic write, migrate) are static and pure so they
 * can be unit-tested without a live GameInstance; the subsystem is thin glue.
 */
UCLASS()
class USaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Current on-disk format version. Bump on a breaking change + add a migration step. */
	static constexpr int32 CurrentSaveVersion = 1;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Live player data — always valid after Initialize. */
	FPlayerSave& GetPlayerData();

	/** Reset state to a fresh new-game save. */
	UFUNCTION(BlueprintCallable, Category = "Save")
	void NewGame();

	/** Persist current data to disk atomically. False on IO failure. */
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool SaveToDisk();

	/** Load (and migrate) data from disk. False if absent or corrupt. */
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool LoadFromDisk();

	UFUNCTION(BlueprintPure, Category = "Save")
	bool HasSaveOnDisk() const;

	// ---- Static, pure helpers (unit-testable without a GameInstance) ----

	/** Default save path under the project's Saved/SaveGames directory. */
	static FString GetSaveFilePath();

	/** Atomic write: serialize to a temp file, then rename over the target. */
	static bool AtomicWriteFile(const FString& FilePath, const TArray<uint8>& Bytes);

	/** Upgrade an older save in place to CurrentSaveVersion (no-op if current). */
	static void MigrateIfNeeded(FPlayerSave& Data);

private:
	UPROPERTY()
	TObjectPtr<UEvoSaveGame> SaveGameObject = nullptr;

	void EnsureSaveObject();
};
