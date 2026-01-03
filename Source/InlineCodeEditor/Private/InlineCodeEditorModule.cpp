// Copyright Yureka. All Rights Reserved.

#include "InlineCodeEditorModule.h"
#include "SIDEPanel.h"

#include "Framework/Docking/TabManager.h"
#include "LevelEditor.h"
#include "Misc/Paths.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

#define LOCTEXT_NAMESPACE "FInlineCodeEditorModule"

static const FName CodeEditorTabName("InlineCodeEditorTab");

void FInlineCodeEditorModule::StartupModule() {
  FLevelEditorModule &LevelEditorModule =
      FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

  RegisterTabSpawner();

  UE_LOG(
      LogTemp, Log,
      TEXT("InlineCodeEditor: ICE IDE loaded - File Tree + Code Editor ready"));
}

void FInlineCodeEditorModule::ShutdownModule() {
  UnregisterTabSpawner();
  UE_LOG(LogTemp, Log, TEXT("InlineCodeEditor: Module shut down"));
}

void FInlineCodeEditorModule::RegisterTabSpawner() {
  FGlobalTabmanager::Get()
      ->RegisterNomadTabSpawner(
          CodeEditorTabName,
          FOnSpawnTab::CreateRaw(this,
                                 &FInlineCodeEditorModule::SpawnCodeEditorTab))
      .SetDisplayName(LOCTEXT("CodeEditorTabTitle", "ICE - Inline Code Editor"))
      .SetTooltipText(LOCTEXT("CodeEditorTabTooltip",
                              "Inline Code Editor IDE with File Explorer"))
      .SetGroup(
          WorkspaceMenu::GetMenuStructure().GetDeveloperToolsMiscCategory())
      .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(),
                          "LevelEditor.Tabs.Viewports"));
}

void FInlineCodeEditorModule::UnregisterTabSpawner() {
  FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(CodeEditorTabName);
}

TSharedRef<SDockTab>
FInlineCodeEditorModule::SpawnCodeEditorTab(const FSpawnTabArgs &SpawnTabArgs) {
  // Create the full IDE panel with file tree and code editor
  TSharedRef<SIDEPanel> IDEPanel =
      SNew(SIDEPanel).DefaultRootPath(FPaths::ProjectDir());

  TSharedRef<SDockTab> DockTab =
      SNew(SDockTab)
          .TabRole(ETabRole::NomadTab)
          .Label(LOCTEXT("CodeEditorTabLabel", "ICE"))[IDEPanel];

  return DockTab;
}

void FInlineCodeEditorModule::StartVSCodiumServer() {
  // Now handled by SCodeEditorTab
}

void FInlineCodeEditorModule::StopVSCodiumServer() {
  // Now handled by SCodeEditorTab destructor
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FInlineCodeEditorModule, InlineCodeEditor)
