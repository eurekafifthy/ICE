// Copyright Yureka. All Rights Reserved.

#include "FileTreeIconManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"

FFileTreeIconManager &FFileTreeIconManager::Get() {
  static FFileTreeIconManager Instance;
  return Instance;
}

FString FFileTreeIconManager::GetIconBasePath() const {
  // Get the plugin's base directory
  FString PluginBaseDir = FPaths::ConvertRelativePathToFull(
      FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("InlineCodeEditor")));

  // Return the path to the Icons folder
  return FPaths::Combine(PluginBaseDir, TEXT("Resources"), TEXT("Icons"));
}

TSharedPtr<FSlateVectorImageBrush>
FFileTreeIconManager::LoadSVGIcon(const FString &IconName,
                                  const FVector2D &IconSize) {
  FString IconPath =
      FPaths::Combine(GetIconBasePath(), IconName + TEXT(".svg"));

  if (!FPaths::FileExists(IconPath)) {
    UE_LOG(LogTemp, Warning, TEXT("FileTreeIconManager: Icon not found at %s"),
           *IconPath);
    return nullptr;
  }

  return MakeShared<FSlateVectorImageBrush>(IconPath, IconSize);
}

void FFileTreeIconManager::Initialize() {
  if (bIsInitialized) {
    return;
  }

  const FVector2D IconSize(16.0f, 16.0f);

  // Load folder icons
  FolderIcon = LoadSVGIcon(TEXT("folder"), IconSize);
  FolderOpenIcon = LoadSVGIcon(TEXT("folder_open"), IconSize);

  // Load file type icons
  DefaultFileIcon = LoadSVGIcon(TEXT("file_default"), IconSize);
  CppIcon = LoadSVGIcon(TEXT("file_cpp"), IconSize);
  CIcon = LoadSVGIcon(TEXT("file_c"), IconSize);
  HeaderIcon = LoadSVGIcon(TEXT("file_header"), IconSize);
  CHeaderIcon = LoadSVGIcon(TEXT("file_cheader"), IconSize);
  CSharpIcon = LoadSVGIcon(TEXT("file_csharp"), IconSize);
  JsonIcon = LoadSVGIcon(TEXT("file_json"), IconSize);
  ConfigIcon = LoadSVGIcon(TEXT("file_config"), IconSize);
  TextIcon = LoadSVGIcon(TEXT("file_text"), IconSize);
  MarkdownIcon = LoadSVGIcon(TEXT("file_markdown"), IconSize);
  ImageIcon = LoadSVGIcon(TEXT("file_image"), IconSize);
  XmlIcon = LoadSVGIcon(TEXT("file_xml"), IconSize);
  PythonIcon = LoadSVGIcon(TEXT("file_python"), IconSize);
  JsIcon = LoadSVGIcon(TEXT("file_js"), IconSize);
  TsIcon = LoadSVGIcon(TEXT("file_ts"), IconSize);
  HtmlIcon = LoadSVGIcon(TEXT("file_html"), IconSize);
  CssIcon = LoadSVGIcon(TEXT("file_css"), IconSize);
  ShellIcon = LoadSVGIcon(TEXT("file_shell"), IconSize);
  BatIcon = LoadSVGIcon(TEXT("file_bat"), IconSize);

  // Build extension to icon mapping
  // C++ files
  ExtensionIconMap.Add(TEXT("cpp"), CppIcon);
  ExtensionIconMap.Add(TEXT("cc"), CppIcon);
  ExtensionIconMap.Add(TEXT("cxx"), CppIcon);

  // C files
  ExtensionIconMap.Add(TEXT("c"), CIcon);

  // Header files (C++ style)
  ExtensionIconMap.Add(TEXT("h"), HeaderIcon);
  ExtensionIconMap.Add(TEXT("hpp"), HeaderIcon);
  ExtensionIconMap.Add(TEXT("hxx"), HeaderIcon);
  ExtensionIconMap.Add(TEXT("inl"), HeaderIcon);

  // C# files
  ExtensionIconMap.Add(TEXT("cs"), CSharpIcon);

  // JSON files
  ExtensionIconMap.Add(TEXT("json"), JsonIcon);
  ExtensionIconMap.Add(TEXT("uplugin"), JsonIcon);
  ExtensionIconMap.Add(TEXT("uproject"), JsonIcon);

  // Config files
  ExtensionIconMap.Add(TEXT("ini"), ConfigIcon);
  ExtensionIconMap.Add(TEXT("cfg"), ConfigIcon);
  ExtensionIconMap.Add(TEXT("config"), ConfigIcon);
  ExtensionIconMap.Add(TEXT("conf"), ConfigIcon);
  ExtensionIconMap.Add(TEXT("yaml"), ConfigIcon);
  ExtensionIconMap.Add(TEXT("yml"), ConfigIcon);
  ExtensionIconMap.Add(TEXT("toml"), ConfigIcon);

  // Text files
  ExtensionIconMap.Add(TEXT("txt"), TextIcon);
  ExtensionIconMap.Add(TEXT("log"), TextIcon);

  // Markdown files
  ExtensionIconMap.Add(TEXT("md"), MarkdownIcon);
  ExtensionIconMap.Add(TEXT("markdown"), MarkdownIcon);

  // Image files
  ExtensionIconMap.Add(TEXT("png"), ImageIcon);
  ExtensionIconMap.Add(TEXT("jpg"), ImageIcon);
  ExtensionIconMap.Add(TEXT("jpeg"), ImageIcon);
  ExtensionIconMap.Add(TEXT("bmp"), ImageIcon);
  ExtensionIconMap.Add(TEXT("tga"), ImageIcon);
  ExtensionIconMap.Add(TEXT("ico"), ImageIcon);
  ExtensionIconMap.Add(TEXT("gif"), ImageIcon);
  ExtensionIconMap.Add(TEXT("svg"), ImageIcon);
  ExtensionIconMap.Add(TEXT("webp"), ImageIcon);

  // XML files
  ExtensionIconMap.Add(TEXT("xml"), XmlIcon);

  // Python files
  ExtensionIconMap.Add(TEXT("py"), PythonIcon);
  ExtensionIconMap.Add(TEXT("pyw"), PythonIcon);

  // JavaScript files
  ExtensionIconMap.Add(TEXT("js"), JsIcon);
  ExtensionIconMap.Add(TEXT("mjs"), JsIcon);
  ExtensionIconMap.Add(TEXT("jsx"), JsIcon);

  // TypeScript files
  ExtensionIconMap.Add(TEXT("ts"), TsIcon);
  ExtensionIconMap.Add(TEXT("tsx"), TsIcon);

  // HTML files
  ExtensionIconMap.Add(TEXT("html"), HtmlIcon);
  ExtensionIconMap.Add(TEXT("htm"), HtmlIcon);

  // CSS files
  ExtensionIconMap.Add(TEXT("css"), CssIcon);
  ExtensionIconMap.Add(TEXT("scss"), CssIcon);
  ExtensionIconMap.Add(TEXT("sass"), CssIcon);
  ExtensionIconMap.Add(TEXT("less"), CssIcon);

  // Shell files
  ExtensionIconMap.Add(TEXT("sh"), ShellIcon);
  ExtensionIconMap.Add(TEXT("bash"), ShellIcon);
  ExtensionIconMap.Add(TEXT("zsh"), ShellIcon);

  // Batch files
  ExtensionIconMap.Add(TEXT("bat"), BatIcon);
  ExtensionIconMap.Add(TEXT("cmd"), BatIcon);
  ExtensionIconMap.Add(TEXT("ps1"), BatIcon);

  bIsInitialized = true;

  UE_LOG(LogTemp, Log,
         TEXT("FileTreeIconManager: Initialized with %d extension mappings"),
         ExtensionIconMap.Num());
}

void FFileTreeIconManager::Shutdown() {
  if (!bIsInitialized) {
    return;
  }

  // Clear all icon brushes
  FolderIcon.Reset();
  FolderOpenIcon.Reset();
  DefaultFileIcon.Reset();
  CppIcon.Reset();
  CIcon.Reset();
  HeaderIcon.Reset();
  CHeaderIcon.Reset();
  CSharpIcon.Reset();
  JsonIcon.Reset();
  ConfigIcon.Reset();
  TextIcon.Reset();
  MarkdownIcon.Reset();
  ImageIcon.Reset();
  XmlIcon.Reset();
  PythonIcon.Reset();
  JsIcon.Reset();
  TsIcon.Reset();
  HtmlIcon.Reset();
  CssIcon.Reset();
  ShellIcon.Reset();
  BatIcon.Reset();

  ExtensionIconMap.Empty();
  bIsInitialized = false;
}

const FSlateBrush *FFileTreeIconManager::GetFolderIcon(bool bIsOpen) const {
  if (bIsOpen && FolderOpenIcon.IsValid()) {
    return FolderOpenIcon.Get();
  } else if (FolderIcon.IsValid()) {
    return FolderIcon.Get();
  }

  return nullptr;
}

const FSlateBrush *
FFileTreeIconManager::GetFileIcon(const FString &Extension) const {
  FString LowerExt = Extension.ToLower();

  if (const TSharedPtr<FSlateVectorImageBrush> *FoundIcon =
          ExtensionIconMap.Find(LowerExt)) {
    if (FoundIcon->IsValid()) {
      return FoundIcon->Get();
    }
  }

  // Return default file icon
  if (DefaultFileIcon.IsValid()) {
    return DefaultFileIcon.Get();
  }

  return nullptr;
}
