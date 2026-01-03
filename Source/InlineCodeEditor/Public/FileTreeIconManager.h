// Copyright Yureka. All Rights Reserved.

#pragma once

#include "Brushes/SlateImageBrush.h"
#include "CoreMinimal.h"


/**
 * Manages VS Code-style file icons for the file tree view
 * Uses FSlateVectorImageBrush to render SVG icons
 */
class FFileTreeIconManager {
public:
  /** Get the singleton instance */
  static FFileTreeIconManager &Get();

  /** Initialize icon brushes */
  void Initialize();

  /** Cleanup icon brushes */
  void Shutdown();

  /** Get icon for a directory (open or closed) */
  const FSlateBrush *GetFolderIcon(bool bIsOpen) const;

  /** Get icon for a file based on its extension */
  const FSlateBrush *GetFileIcon(const FString &Extension) const;

  /** Check if the icons have been initialized */
  bool IsInitialized() const { return bIsInitialized; }

private:
  FFileTreeIconManager() = default;
  ~FFileTreeIconManager() = default;

  /** Load an SVG icon from the Resources folder */
  TSharedPtr<FSlateVectorImageBrush>
  LoadSVGIcon(const FString &IconName,
              const FVector2D &IconSize = FVector2D(16.0f, 16.0f));

  /** Get the base path for icons */
  FString GetIconBasePath() const;

private:
  bool bIsInitialized = false;

  // Folder icons
  TSharedPtr<FSlateVectorImageBrush> FolderIcon;
  TSharedPtr<FSlateVectorImageBrush> FolderOpenIcon;

  // File type icons
  TSharedPtr<FSlateVectorImageBrush> DefaultFileIcon;
  TSharedPtr<FSlateVectorImageBrush> CppIcon;
  TSharedPtr<FSlateVectorImageBrush> CIcon;
  TSharedPtr<FSlateVectorImageBrush> HeaderIcon;
  TSharedPtr<FSlateVectorImageBrush> CHeaderIcon;
  TSharedPtr<FSlateVectorImageBrush> CSharpIcon;
  TSharedPtr<FSlateVectorImageBrush> JsonIcon;
  TSharedPtr<FSlateVectorImageBrush> ConfigIcon;
  TSharedPtr<FSlateVectorImageBrush> TextIcon;
  TSharedPtr<FSlateVectorImageBrush> MarkdownIcon;
  TSharedPtr<FSlateVectorImageBrush> ImageIcon;
  TSharedPtr<FSlateVectorImageBrush> XmlIcon;
  TSharedPtr<FSlateVectorImageBrush> PythonIcon;
  TSharedPtr<FSlateVectorImageBrush> JsIcon;
  TSharedPtr<FSlateVectorImageBrush> TsIcon;
  TSharedPtr<FSlateVectorImageBrush> HtmlIcon;
  TSharedPtr<FSlateVectorImageBrush> CssIcon;
  TSharedPtr<FSlateVectorImageBrush> ShellIcon;
  TSharedPtr<FSlateVectorImageBrush> BatIcon;

  // Extension to icon mapping
  TMap<FString, TSharedPtr<FSlateVectorImageBrush>> ExtensionIconMap;
};
