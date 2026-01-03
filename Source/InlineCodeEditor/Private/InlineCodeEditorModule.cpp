// Copyright InlineEditor Team. All Rights Reserved.

#include "InlineCodeEditorModule.h"
#include "SCodeEditorTab.h"

#include "Framework/Docking/TabManager.h"
#include "LevelEditor.h"
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

  UE_LOG(LogTemp, Log,
         TEXT("InlineCodeEditor: Module loaded - Monaco Editor ready"));
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
      .SetDisplayName(LOCTEXT("CodeEditorTabTitle", "Code Editor"))
      .SetTooltipText(
          LOCTEXT("CodeEditorTabTooltip", "Inline Monaco code editor"))
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
  TSharedRef<SCodeEditorTab> EditorWidget = SNew(SCodeEditorTab);

  TSharedRef<SDockTab> DockTab =
      SNew(SDockTab)
          .TabRole(ETabRole::NomadTab)
          .Label(LOCTEXT("CodeEditorTabLabel", "Code Editor"))[EditorWidget];

  // Pass tab reference so widget can check IsForeground()
  EditorWidget->SetParentTab(DockTab);

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
