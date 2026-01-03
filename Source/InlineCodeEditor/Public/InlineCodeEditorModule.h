// Copyright InlineEditor Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FInlineCodeEditorModule : public IModuleInterface {
public:
  virtual void StartupModule() override;
  virtual void ShutdownModule() override;

private:
  void RegisterTabSpawner();
  void UnregisterTabSpawner();
  TSharedRef<class SDockTab>
  SpawnCodeEditorTab(const class FSpawnTabArgs &SpawnTabArgs);

  // Legacy - kept for interface compatibility
  void StartVSCodiumServer();
  void StopVSCodiumServer();

  static constexpr int32 ServerPort = 13370;
};
