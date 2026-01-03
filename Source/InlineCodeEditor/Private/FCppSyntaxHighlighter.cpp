// Copyright Yureka. All Rights Reserved.

#include "FCppSyntaxHighlighter.h"
#include "Framework/Text/IRun.h"
#include "Framework/Text/SlateTextRun.h"
#include "Framework/Text/TextLayout.h"

//////////////////////////////////////////////////////////////////////////
// FCppSyntaxTokenizer

TSharedRef<FCppSyntaxTokenizer> FCppSyntaxTokenizer::Create() {
  return MakeShareable(new FCppSyntaxTokenizer());
}

FCppSyntaxTokenizer::FCppSyntaxTokenizer() {
  // C++ Keywords (excluding control flow which are separate)
  Keywords = {
      TEXT("alignas"),
      TEXT("alignof"),
      TEXT("and"),
      TEXT("and_eq"),
      TEXT("asm"),
      TEXT("auto"),
      TEXT("bitand"),
      TEXT("bitor"),
      TEXT("class"),
      TEXT("compl"),
      TEXT("concept"),
      TEXT("const"),
      TEXT("consteval"),
      TEXT("constexpr"),
      TEXT("constinit"),
      TEXT("const_cast"),
      TEXT("decltype"),
      TEXT("delete"),
      TEXT("dynamic_cast"),
      TEXT("enum"),
      TEXT("explicit"),
      TEXT("export"),
      TEXT("extern"),
      TEXT("false"),
      TEXT("final"),
      TEXT("friend"),
      TEXT("inline"),
      TEXT("mutable"),
      TEXT("namespace"),
      TEXT("new"),
      TEXT("noexcept"),
      TEXT("not"),
      TEXT("not_eq"),
      TEXT("nullptr"),
      TEXT("operator"),
      TEXT("or"),
      TEXT("or_eq"),
      TEXT("override"),
      TEXT("private"),
      TEXT("protected"),
      TEXT("public"),
      TEXT("register"),
      TEXT("reinterpret_cast"),
      TEXT("requires"),
      TEXT("sizeof"),
      TEXT("static"),
      TEXT("static_assert"),
      TEXT("static_cast"),
      TEXT("struct"),
      TEXT("template"),
      TEXT("this"),
      TEXT("thread_local"),
      TEXT("true"),
      TEXT("typedef"),
      TEXT("typeid"),
      TEXT("typename"),
      TEXT("union"),
      TEXT("using"),
      TEXT("virtual"),
      TEXT("volatile"),
      TEXT("xor"),
      TEXT("xor_eq"),
  };

  // Control flow keywords (highlighted differently in Monaco)
  ControlFlowKeywords = {
      TEXT("break"),     TEXT("case"),     TEXT("catch"),    TEXT("co_await"),
      TEXT("co_return"), TEXT("co_yield"), TEXT("continue"), TEXT("default"),
      TEXT("do"),        TEXT("else"),     TEXT("for"),      TEXT("goto"),
      TEXT("if"),        TEXT("return"),   TEXT("switch"),   TEXT("throw"),
      TEXT("try"),       TEXT("while"),
  };

  // C++ Built-in types
  BuiltInTypes = {
      TEXT("bool"),     TEXT("char"),     TEXT("char8_t"),   TEXT("char16_t"),
      TEXT("char32_t"), TEXT("double"),   TEXT("float"),     TEXT("int"),
      TEXT("int8_t"),   TEXT("int16_t"),  TEXT("int32_t"),   TEXT("int64_t"),
      TEXT("intptr_t"), TEXT("long"),     TEXT("ptrdiff_t"), TEXT("short"),
      TEXT("signed"),   TEXT("size_t"),   TEXT("uint8_t"),   TEXT("uint16_t"),
      TEXT("uint32_t"), TEXT("uint64_t"), TEXT("uintptr_t"), TEXT("unsigned"),
      TEXT("void"),     TEXT("wchar_t"),
  };

  // Unreal Engine types (treated like types in Monaco)
  UnrealTypes = {
      TEXT("FString"),
      TEXT("FName"),
      TEXT("FText"),
      TEXT("FVector"),
      TEXT("FVector2D"),
      TEXT("FVector4"),
      TEXT("FRotator"),
      TEXT("FTransform"),
      TEXT("FQuat"),
      TEXT("FMatrix"),
      TEXT("FColor"),
      TEXT("FLinearColor"),
      TEXT("FIntPoint"),
      TEXT("FIntVector"),
      TEXT("FBox"),
      TEXT("FBox2D"),
      TEXT("FSphere"),
      TEXT("FPlane"),
      TEXT("FMargin"),
      TEXT("FSlateColor"),
      TEXT("int8"),
      TEXT("int16"),
      TEXT("int32"),
      TEXT("int64"),
      TEXT("uint8"),
      TEXT("uint16"),
      TEXT("uint32"),
      TEXT("uint64"),
      TEXT("TArray"),
      TEXT("TMap"),
      TEXT("TSet"),
      TEXT("TMultiMap"),
      TEXT("TPair"),
      TEXT("TTuple"),
      TEXT("TSharedPtr"),
      TEXT("TSharedRef"),
      TEXT("TWeakPtr"),
      TEXT("TUniquePtr"),
      TEXT("TOptional"),
      TEXT("TFunction"),
      TEXT("TDelegate"),
      TEXT("TSubclassOf"),
      TEXT("TSoftObjectPtr"),
      TEXT("TSoftClassPtr"),
      TEXT("TObjectPtr"),
      TEXT("FObjectPtr"),
      TEXT("TAttribute"),
      TEXT("UObject"),
      TEXT("AActor"),
      TEXT("APawn"),
      TEXT("ACharacter"),
      TEXT("AController"),
      TEXT("APlayerController"),
      TEXT("AGameModeBase"),
      TEXT("AGameStateBase"),
      TEXT("UActorComponent"),
      TEXT("USceneComponent"),
      TEXT("UPrimitiveComponent"),
      TEXT("UStaticMeshComponent"),
      TEXT("USkeletalMeshComponent"),
      TEXT("UWorld"),
      TEXT("ULevel"),
      TEXT("UGameInstance"),
      TEXT("UClass"),
      TEXT("UStruct"),
      TEXT("UEnum"),
      TEXT("UField"),
      TEXT("UProperty"),
      TEXT("UFunction"),
      TEXT("UPackage"),
      TEXT("UAssetManager"),
      TEXT("UWidget"),
      TEXT("UUserWidget"),
      TEXT("UCanvasPanel"),
      TEXT("USlateWidgetStyleAsset"),
      TEXT("SWidget"),
      TEXT("SCompoundWidget"),
      TEXT("SLeafWidget"),
      TEXT("SPanel"),
      TEXT("SScrollBox"),
      TEXT("SBorder"),
      TEXT("SButton"),
      TEXT("STextBlock"),
      TEXT("SEditableText"),
      TEXT("SMultiLineEditableText"),
      TEXT("SListView"),
      TEXT("STreeView"),
      TEXT("FSlateApplication"),
      TEXT("ISlateStyle"),
      TEXT("FTextBlockStyle"),
      TEXT("FButtonStyle"),
      TEXT("FRunInfo"),
      TEXT("FTextRange"),
      TEXT("FTextLocation"),
  };

  // Unreal Engine macros (highlighted like functions in Monaco)
  UnrealMacros = {
      TEXT("UCLASS"),
      TEXT("USTRUCT"),
      TEXT("UENUM"),
      TEXT("UINTERFACE"),
      TEXT("UPROPERTY"),
      TEXT("UFUNCTION"),
      TEXT("UMETA"),
      TEXT("GENERATED_BODY"),
      TEXT("GENERATED_UCLASS_BODY"),
      TEXT("GENERATED_USTRUCT_BODY"),
      TEXT("DECLARE_DELEGATE"),
      TEXT("DECLARE_DELEGATE_OneParam"),
      TEXT("DECLARE_DELEGATE_TwoParams"),
      TEXT("DECLARE_DELEGATE_RetVal"),
      TEXT("DECLARE_MULTICAST_DELEGATE"),
      TEXT("DECLARE_DYNAMIC_DELEGATE"),
      TEXT("DECLARE_DYNAMIC_MULTICAST_DELEGATE"),
      TEXT("UE_LOG"),
      TEXT("UE_LOGFMT"),
      TEXT("UE_CLOG"),
      TEXT("UE_LOGFMT_EX"),
      TEXT("check"),
      TEXT("checkf"),
      TEXT("checkNoEntry"),
      TEXT("checkNoReentry"),
      TEXT("checkNoRecursion"),
      TEXT("verify"),
      TEXT("verifyf"),
      TEXT("ensure"),
      TEXT("ensureMsgf"),
      TEXT("ensureAlways"),
      TEXT("ensureAlwaysMsgf"),
      TEXT("IMPLEMENT_MODULE"),
      TEXT("IMPLEMENT_GAME_MODULE"),
      TEXT("IMPLEMENT_PRIMARY_GAME_MODULE"),
      TEXT("LOCTEXT"),
      TEXT("LOCTEXT_NAMESPACE"),
      TEXT("NSLOCTEXT"),
      TEXT("INVTEXT"),
      TEXT("TEXT"),
      TEXT("TEXTVIEW"),
      TEXT("TCHAR_TO_ANSI"),
      TEXT("ANSI_TO_TCHAR"),
      TEXT("TCHAR_TO_UTF8"),
      TEXT("UTF8_TO_TCHAR"),
      TEXT("WITH_EDITOR"),
      TEXT("WITH_EDITORONLY_DATA"),
      TEXT("WITH_ENGINE"),
      TEXT("WITH_SERVER_CODE"),
      TEXT("FORCEINLINE"),
      TEXT("FORCENOINLINE"),
      TEXT("PURE_VIRTUAL"),
      TEXT("ABSTRACT_MODULE"),
      TEXT("SLATE_BEGIN_ARGS"),
      TEXT("SLATE_END_ARGS"),
      TEXT("SLATE_ARGUMENT"),
      TEXT("SLATE_ATTRIBUTE"),
      TEXT("SLATE_EVENT"),
      TEXT("SLATE_STYLE_ARGUMENT"),
      TEXT("SLATE_DEFAULT_SLOT"),
      TEXT("SLATE_NAMED_SLOT"),
      TEXT("SLATE_SUPPORTS_SLOT"),
      TEXT("SNew"),
      TEXT("SAssignNew"),
      TEXT("MakeShared"),
      TEXT("MakeShareable"),
      TEXT("MakeUnique"),
      TEXT("StaticCastSharedPtr"),
      TEXT("StaticCastSharedRef"),
      TEXT("ConstCastSharedPtr"),
      TEXT("ConstCastSharedRef"),
      TEXT("Cast"),
      TEXT("CastChecked"),
      TEXT("ExactCast"),
      TEXT("IsValid"),
      TEXT("IsA"),
      TEXT("GetClass"),
      TEXT("GetName"),
      TEXT("GetFName"),
      TEXT("GetOuter"),
      TEXT("GetWorld"),
      TEXT("NewObject"),
      TEXT("CreateDefaultSubobject"),
      TEXT("DuplicateObject"),
      TEXT("ConstructorHelpers"),
      TEXT("FMath"),
      TEXT("UE_ARRAY_COUNT"),
      TEXT("ARRAY_COUNT"),
      TEXT("INDEX_NONE"),
  };

  // Preprocessor directives
  PreProcessorDirectives = {
      TEXT("include"), TEXT("define"), TEXT("undef"),     TEXT("if"),
      TEXT("ifdef"),   TEXT("ifndef"), TEXT("else"),      TEXT("elif"),
      TEXT("endif"),   TEXT("error"),  TEXT("warning"),   TEXT("pragma"),
      TEXT("line"),    TEXT("region"), TEXT("endregion"),
  };
}

bool FCppSyntaxTokenizer::IsOperatorChar(TCHAR C) const {
  return C == '+' || C == '-' || C == '*' || C == '/' || C == '%' || C == '&' ||
         C == '|' || C == '^' || C == '~' || C == '!' || C == '<' || C == '>' ||
         C == '=' || C == '?' || C == ':';
}

void FCppSyntaxTokenizer::Process(TArray<FTokenizedLine> &OutTokenizedLines,
                                  const FString &Input) {
  OutTokenizedLines.Empty();

  // Split input into lines
  TArray<FTextRange> LineRanges;
  FTextRange::CalculateLineRangesFromString(Input, LineRanges);

  // Reset block comment state at start of full parse
  bInBlockComment = false;

  for (const FTextRange &LineRange : LineRanges) {
    FTokenizedLine TokenizedLine;
    TokenizedLine.Range = LineRange;

    if (LineRange.IsEmpty()) {
      OutTokenizedLines.Add(TokenizedLine);
      continue;
    }

    int32 CurrentPos = LineRange.BeginIndex;
    int32 LineEnd = LineRange.EndIndex;

    // Context tracking for better tokenization
    bool bAfterClassKeyword = false;
    bool bAfterNamespaceKeyword = false;
    bool bAfterScopeResolution = false;
    bool bIsPreprocessorLine = false;

    while (CurrentPos < LineEnd) {
      TCHAR CurrentChar = Input[CurrentPos];
      TCHAR NextChar = (CurrentPos + 1 < LineEnd) ? Input[CurrentPos + 1] : 0;
      int32 TokenStart = CurrentPos;

      //=====================================================================
      // 1. BLOCK COMMENT (continued from previous line)
      //=====================================================================
      if (bInBlockComment) {
        int32 CommentEnd = Input.Find(TEXT("*/"), ESearchCase::CaseSensitive,
                                      ESearchDir::FromStart, CurrentPos);
        if (CommentEnd != INDEX_NONE && CommentEnd < LineEnd) {
          TokenizedLine.Tokens.Add(
              FToken(static_cast<ETokenType>(ECppTokenType::Comment),
                     FTextRange(TokenStart, CommentEnd + 2)));
          CurrentPos = CommentEnd + 2;
          bInBlockComment = false;
        } else {
          TokenizedLine.Tokens.Add(
              FToken(static_cast<ETokenType>(ECppTokenType::Comment),
                     FTextRange(TokenStart, LineEnd)));
          CurrentPos = LineEnd;
        }
        continue;
      }

      //=====================================================================
      // 2. WHITESPACE - skip but preserve position
      //=====================================================================
      if (FChar::IsWhitespace(CurrentChar)) {
        CurrentPos++;
        continue;
      }

      //=====================================================================
      // 3. BLOCK COMMENT START
      //=====================================================================
      if (CurrentChar == '/' && NextChar == '*') {
        bInBlockComment = true;
        int32 CommentEnd = Input.Find(TEXT("*/"), ESearchCase::CaseSensitive,
                                      ESearchDir::FromStart, CurrentPos + 2);
        if (CommentEnd != INDEX_NONE && CommentEnd < LineEnd) {
          TokenizedLine.Tokens.Add(
              FToken(static_cast<ETokenType>(ECppTokenType::Comment),
                     FTextRange(TokenStart, CommentEnd + 2)));
          CurrentPos = CommentEnd + 2;
          bInBlockComment = false;
        } else {
          TokenizedLine.Tokens.Add(
              FToken(static_cast<ETokenType>(ECppTokenType::Comment),
                     FTextRange(TokenStart, LineEnd)));
          CurrentPos = LineEnd;
        }
        continue;
      }

      //=====================================================================
      // 4. LINE COMMENT
      //=====================================================================
      if (CurrentChar == '/' && NextChar == '/') {
        TokenizedLine.Tokens.Add(
            FToken(static_cast<ETokenType>(ECppTokenType::Comment),
                   FTextRange(TokenStart, LineEnd)));
        CurrentPos = LineEnd;
        continue;
      }

      //=====================================================================
      // 5. PREPROCESSOR DIRECTIVE
      //=====================================================================
      if (CurrentChar == '#') {
        bIsPreprocessorLine = true;
        CurrentPos++; // Skip #

        // Skip whitespace after #
        while (CurrentPos < LineEnd && FChar::IsWhitespace(Input[CurrentPos])) {
          CurrentPos++;
        }

        // Read directive name
        int32 DirectiveStart = CurrentPos;
        while (CurrentPos < LineEnd && FChar::IsAlpha(Input[CurrentPos])) {
          CurrentPos++;
        }

        FString Directive =
            Input.Mid(DirectiveStart, CurrentPos - DirectiveStart).ToLower();

        // Add the # and directive as preprocessor token
        TokenizedLine.Tokens.Add(
            FToken(static_cast<ETokenType>(ECppTokenType::PreProcessor),
                   FTextRange(TokenStart, CurrentPos)));

        // Check if it's #include
        if (Directive == TEXT("include")) {
          // Skip whitespace
          while (CurrentPos < LineEnd &&
                 FChar::IsWhitespace(Input[CurrentPos])) {
            CurrentPos++;
          }
          // Capture the include path (either <...> or "...")
          if (CurrentPos < LineEnd) {
            TCHAR PathDelim = Input[CurrentPos];
            if (PathDelim == '<' || PathDelim == '"') {
              TCHAR EndDelim = (PathDelim == '<') ? '>' : '"';
              int32 PathStart = CurrentPos;
              CurrentPos++;
              while (CurrentPos < LineEnd && Input[CurrentPos] != EndDelim) {
                CurrentPos++;
              }
              if (CurrentPos < LineEnd) {
                CurrentPos++; // Include the closing delimiter
              }
              TokenizedLine.Tokens.Add(
                  FToken(static_cast<ETokenType>(ECppTokenType::IncludePath),
                         FTextRange(PathStart, CurrentPos)));
            }
          }
        }
        // Rest of preprocessor line is normal (for now)
        continue;
      }

      //=====================================================================
      // 6. STRING LITERALS
      //=====================================================================
      if (CurrentChar == '"' || CurrentChar == '\'') {
        TCHAR Delimiter = CurrentChar;
        CurrentPos++;
        while (CurrentPos < LineEnd) {
          TCHAR C = Input[CurrentPos];
          if (C == '\\' && CurrentPos + 1 < LineEnd) {
            CurrentPos += 2; // Skip escape sequence
          } else if (C == Delimiter) {
            CurrentPos++;
            break;
          } else {
            CurrentPos++;
          }
        }
        TokenizedLine.Tokens.Add(
            FToken(static_cast<ETokenType>(ECppTokenType::String),
                   FTextRange(TokenStart, CurrentPos)));
        continue;
      }

      //=====================================================================
      // 7. RAW STRING LITERALS (R"(...)")
      //=====================================================================
      if (CurrentChar == 'R' && NextChar == '"') {
        CurrentPos += 2; // Skip R"
        // Find delimiter (text between R" and ()
        int32 DelimStart = CurrentPos;
        while (CurrentPos < LineEnd && Input[CurrentPos] != '(') {
          CurrentPos++;
        }
        FString Delim = Input.Mid(DelimStart, CurrentPos - DelimStart);
        FString EndPattern = TEXT(")") + Delim + TEXT("\"");
        CurrentPos++; // Skip (

        // Find end of raw string
        int32 EndPos = Input.Find(EndPattern, ESearchCase::CaseSensitive,
                                  ESearchDir::FromStart, CurrentPos);
        if (EndPos != INDEX_NONE) {
          CurrentPos = EndPos + EndPattern.Len();
        } else {
          CurrentPos = LineEnd; // Unterminated, take rest of line
        }
        TokenizedLine.Tokens.Add(
            FToken(static_cast<ETokenType>(ECppTokenType::String),
                   FTextRange(TokenStart, CurrentPos)));
        continue;
      }

      //=====================================================================
      // 8. NUMBERS
      //=====================================================================
      if (FChar::IsDigit(CurrentChar) ||
          (CurrentChar == '.' && CurrentPos + 1 < LineEnd &&
           FChar::IsDigit(Input[CurrentPos + 1]))) {
        // Hex (0x) or binary (0b)
        if (CurrentChar == '0' && CurrentPos + 1 < LineEnd) {
          TCHAR Prefix = FChar::ToLower(Input[CurrentPos + 1]);
          if (Prefix == 'x' || Prefix == 'b') {
            CurrentPos += 2;
            while (CurrentPos < LineEnd) {
              TCHAR C = Input[CurrentPos];
              if (FChar::IsAlnum(C) || C == '\'') {
                CurrentPos++;
              } else {
                break;
              }
            }
            TokenizedLine.Tokens.Add(
                FToken(static_cast<ETokenType>(ECppTokenType::Number),
                       FTextRange(TokenStart, CurrentPos)));
            continue;
          }
        }
        // Decimal/Float
        bool bHasDot = false;
        bool bHasExp = false;
        while (CurrentPos < LineEnd) {
          TCHAR C = Input[CurrentPos];
          if (FChar::IsDigit(C) || C == '\'') {
            CurrentPos++;
          } else if (C == '.' && !bHasDot) {
            bHasDot = true;
            CurrentPos++;
          } else if ((C == 'e' || C == 'E') && !bHasExp) {
            bHasExp = true;
            CurrentPos++;
            if (CurrentPos < LineEnd &&
                (Input[CurrentPos] == '+' || Input[CurrentPos] == '-')) {
              CurrentPos++;
            }
          } else if (C == 'f' || C == 'F' || C == 'l' || C == 'L' || C == 'u' ||
                     C == 'U') {
            CurrentPos++;
          } else {
            break;
          }
        }
        TokenizedLine.Tokens.Add(
            FToken(static_cast<ETokenType>(ECppTokenType::Number),
                   FTextRange(TokenStart, CurrentPos)));
        continue;
      }

      //=====================================================================
      // 9. SCOPE RESOLUTION ::
      //=====================================================================
      if (CurrentChar == ':' && NextChar == ':') {
        TokenizedLine.Tokens.Add(
            FToken(static_cast<ETokenType>(ECppTokenType::Operator),
                   FTextRange(TokenStart, CurrentPos + 2)));
        CurrentPos += 2;
        bAfterScopeResolution = true;
        continue;
      }

      //=====================================================================
      // 10. MEMBER ACCESS (. and ->)
      //=====================================================================
      if (CurrentChar == '.' || (CurrentChar == '-' && NextChar == '>')) {
        int32 TokenEnd = CurrentPos + (CurrentChar == '-' ? 2 : 1);
        TokenizedLine.Tokens.Add(
            FToken(static_cast<ETokenType>(ECppTokenType::MemberAccess),
                   FTextRange(TokenStart, TokenEnd)));
        CurrentPos = TokenEnd;
        continue;
      }

      //=====================================================================
      // 11. IDENTIFIERS (keywords, types, functions, etc.)
      //=====================================================================
      if (FChar::IsAlpha(CurrentChar) || CurrentChar == '_') {
        while (CurrentPos < LineEnd) {
          TCHAR C = Input[CurrentPos];
          if (FChar::IsAlnum(C) || C == '_') {
            CurrentPos++;
          } else {
            break;
          }
        }

        FString TokenText = Input.Mid(TokenStart, CurrentPos - TokenStart);

        // Look ahead to determine if this is a function call
        int32 LookAhead = CurrentPos;
        while (LookAhead < LineEnd && FChar::IsWhitespace(Input[LookAhead])) {
          LookAhead++;
        }
        bool bFollowedByParen =
            (LookAhead < LineEnd && Input[LookAhead] == '(');
        bool bFollowedByTemplate =
            (LookAhead < LineEnd && Input[LookAhead] == '<');

        // Get token type with context
        ECppTokenType TokenType =
            GetTokenType(TokenText, bFollowedByParen, bAfterClassKeyword,
                         bAfterNamespaceKeyword, bAfterScopeResolution);

        // Update context for next token
        bool bWasClassKeyword =
            (TokenText == TEXT("class") || TokenText == TEXT("struct") ||
             TokenText == TEXT("enum"));
        bool bWasNamespaceKeyword = (TokenText == TEXT("namespace"));

        bAfterClassKeyword = bWasClassKeyword;
        bAfterNamespaceKeyword = bWasNamespaceKeyword;
        bAfterScopeResolution = false;

        TokenizedLine.Tokens.Add(FToken(static_cast<ETokenType>(TokenType),
                                        FTextRange(TokenStart, CurrentPos)));
        continue;
      }

      //=====================================================================
      // 12. MULTI-CHARACTER OPERATORS
      //=====================================================================
      if (IsOperatorChar(CurrentChar)) {
        // Try to match multi-char operators
        int32 OpEnd = CurrentPos + 1;
        if (OpEnd < LineEnd) {
          FString TwoChar = Input.Mid(CurrentPos, 2);
          if (TwoChar == TEXT("<<") || TwoChar == TEXT(">>") ||
              TwoChar == TEXT("<=") || TwoChar == TEXT(">=") ||
              TwoChar == TEXT("==") || TwoChar == TEXT("!=") ||
              TwoChar == TEXT("&&") || TwoChar == TEXT("||") ||
              TwoChar == TEXT("+=") || TwoChar == TEXT("-=") ||
              TwoChar == TEXT("*=") || TwoChar == TEXT("/=") ||
              TwoChar == TEXT("%=") || TwoChar == TEXT("&=") ||
              TwoChar == TEXT("|=") || TwoChar == TEXT("^=") ||
              TwoChar == TEXT("++") || TwoChar == TEXT("--")) {
            OpEnd = CurrentPos + 2;
            // Check for three-char operators
            if (OpEnd < LineEnd) {
              FString ThreeChar = Input.Mid(CurrentPos, 3);
              if (ThreeChar == TEXT("<<=") || ThreeChar == TEXT(">>=") ||
                  ThreeChar == TEXT("<=>")) {
                OpEnd = CurrentPos + 3;
              }
            }
          }
        }
        TokenizedLine.Tokens.Add(
            FToken(static_cast<ETokenType>(ECppTokenType::Operator),
                   FTextRange(CurrentPos, OpEnd)));
        CurrentPos = OpEnd;
        continue;
      }

      //=====================================================================
      // 13. PUNCTUATION ( { } [ ] ( ) ; , )
      //=====================================================================
      if (CurrentChar == '{' || CurrentChar == '}' || CurrentChar == '[' ||
          CurrentChar == ']' || CurrentChar == '(' || CurrentChar == ')' ||
          CurrentChar == ';' || CurrentChar == ',') {
        TokenizedLine.Tokens.Add(
            FToken(static_cast<ETokenType>(ECppTokenType::Punctuation),
                   FTextRange(CurrentPos, CurrentPos + 1)));
        CurrentPos++;
        continue;
      }

      //=====================================================================
      // 14. TEMPLATE ANGLE BRACKETS (< and >)
      //=====================================================================
      if (CurrentChar == '<' || CurrentChar == '>') {
        // Could be comparison or template - treat as operator for simplicity
        TokenizedLine.Tokens.Add(
            FToken(static_cast<ETokenType>(ECppTokenType::Operator),
                   FTextRange(CurrentPos, CurrentPos + 1)));
        CurrentPos++;
        continue;
      }

      //=====================================================================
      // 15. ANY OTHER CHARACTER
      //=====================================================================
      TokenizedLine.Tokens.Add(
          FToken(static_cast<ETokenType>(ECppTokenType::Normal),
                 FTextRange(CurrentPos, CurrentPos + 1)));
      CurrentPos++;
    }

    OutTokenizedLines.Add(TokenizedLine);
  }
}

ECppTokenType
FCppSyntaxTokenizer::GetTokenType(const FString &Token, bool bFollowedByParen,
                                  bool bAfterClassKeyword, bool bAfterNamespace,
                                  bool bAfterScopeResolution) const {
  // After 'class', 'struct', 'enum' - this is a class name
  if (bAfterClassKeyword) {
    return ECppTokenType::ClassName;
  }

  // After 'namespace' - this is a namespace name
  if (bAfterNamespace) {
    return ECppTokenType::Namespace;
  }

  // After :: - could be namespace or type
  if (bAfterScopeResolution) {
    if (UnrealTypes.Contains(Token) || BuiltInTypes.Contains(Token)) {
      return ECppTokenType::Type;
    }
    if (UnrealMacros.Contains(Token)) {
      return ECppTokenType::UnrealMacro;
    }
    if (bFollowedByParen) {
      return ECppTokenType::FunctionCall;
    }
    return ECppTokenType::Namespace;
  }

  // Control flow keywords (purple in Monaco)
  if (ControlFlowKeywords.Contains(Token)) {
    return ECppTokenType::ControlFlow;
  }

  // Regular keywords (blue in Monaco)
  if (Keywords.Contains(Token)) {
    return ECppTokenType::Keyword;
  }

  // Built-in types
  if (BuiltInTypes.Contains(Token)) {
    return ECppTokenType::Type;
  }

  // Unreal types
  if (UnrealTypes.Contains(Token)) {
    return ECppTokenType::Type;
  }

  // Unreal macros (yellow in Monaco like functions)
  if (UnrealMacros.Contains(Token)) {
    return ECppTokenType::UnrealMacro;
  }

  // Function call (identifier followed by parenthesis)
  if (bFollowedByParen) {
    return ECppTokenType::FunctionCall;
  }

  // Default: normal identifier
  return ECppTokenType::Normal;
}

//////////////////////////////////////////////////////////////////////////
// FCppSyntaxHighlighter

TSharedRef<FCppSyntaxHighlighter> FCppSyntaxHighlighter::Create() {
  return MakeShareable(
      new FCppSyntaxHighlighter(FCppSyntaxTokenizer::Create()));
}

FCppSyntaxHighlighter::FCppSyntaxHighlighter(
    TSharedPtr<ISyntaxTokenizer> InTokenizer)
    : FSyntaxHighlighterTextLayoutMarshaller(InTokenizer) {

  //=========================================================================
  // Monaco Dark+ Theme Colors
  // Reference:
  // https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/dark_plus.json
  //=========================================================================

  FSlateFontInfo MonoFont = FCoreStyle::GetDefaultFontStyle("Mono", 10);

  // Normal text - #D4D4D4
  NormalTextStyle =
      FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");
  NormalTextStyle.SetFont(MonoFont);
  NormalTextStyle.SetColorAndOpacity(
      FLinearColor::FromSRGBColor(FColor::FromHex("D4D4D4FF")));

  // Comments - #6A9955 (green)
  CommentTextStyle = NormalTextStyle;
  CommentTextStyle.SetColorAndOpacity(
      FLinearColor::FromSRGBColor(FColor::FromHex("6A9955FF")));

  // Strings - #CE9178 (orange/salmon)
  StringTextStyle = NormalTextStyle;
  StringTextStyle.SetColorAndOpacity(
      FLinearColor::FromSRGBColor(FColor::FromHex("CE9178FF")));

  // Keywords - #569CD6 (blue)
  KeywordTextStyle = NormalTextStyle;
  KeywordTextStyle.SetColorAndOpacity(
      FLinearColor::FromSRGBColor(FColor::FromHex("569CD6FF")));

  // Control flow (return, if, for, etc.) - #C586C0 (purple/magenta)
  ControlFlowStyle = NormalTextStyle;
  ControlFlowStyle.SetColorAndOpacity(
      FLinearColor::FromSRGBColor(FColor::FromHex("C586C0FF")));

  // Types - #4EC9B0 (teal/cyan)
  TypeTextStyle = NormalTextStyle;
  TypeTextStyle.SetColorAndOpacity(
      FLinearColor::FromSRGBColor(FColor::FromHex("4EC9B0FF")));

  // Macros / Functions - #DCDCAA (yellow)
  MacroTextStyle = NormalTextStyle;
  MacroTextStyle.SetColorAndOpacity(
      FLinearColor::FromSRGBColor(FColor::FromHex("DCDCAAFF")));

  // Function calls - #DCDCAA (yellow, same as macros)
  FunctionCallStyle = NormalTextStyle;
  FunctionCallStyle.SetColorAndOpacity(
      FLinearColor::FromSRGBColor(FColor::FromHex("DCDCAAFF")));

  // Numbers - #B5CEA8 (light green)
  NumberTextStyle = NormalTextStyle;
  NumberTextStyle.SetColorAndOpacity(
      FLinearColor::FromSRGBColor(FColor::FromHex("B5CEA8FF")));

  // Operators - #D4D4D4 (same as normal)
  OperatorTextStyle = NormalTextStyle;
  OperatorTextStyle.SetColorAndOpacity(
      FLinearColor::FromSRGBColor(FColor::FromHex("D4D4D4FF")));

  // Punctuation - #808080 (dim gray) - slightly dimmer
  PunctuationStyle = NormalTextStyle;
  PunctuationStyle.SetColorAndOpacity(
      FLinearColor::FromSRGBColor(FColor::FromHex("D4D4D4FF")));

  // Class names - #4EC9B0 (teal, same as types)
  ClassNameStyle = NormalTextStyle;
  ClassNameStyle.SetColorAndOpacity(
      FLinearColor::FromSRGBColor(FColor::FromHex("4EC9B0FF")));

  // Namespaces - #4EC9B0 (teal)
  NamespaceStyle = NormalTextStyle;
  NamespaceStyle.SetColorAndOpacity(
      FLinearColor::FromSRGBColor(FColor::FromHex("4EC9B0FF")));

  // Preprocessor - #C586C0 (purple)
  PreProcessorStyle = NormalTextStyle;
  PreProcessorStyle.SetColorAndOpacity(
      FLinearColor::FromSRGBColor(FColor::FromHex("C586C0FF")));

  // Include paths - #CE9178 (orange, same as strings)
  IncludePathStyle = NormalTextStyle;
  IncludePathStyle.SetColorAndOpacity(
      FLinearColor::FromSRGBColor(FColor::FromHex("CE9178FF")));
}

void FCppSyntaxHighlighter::ParseTokens(
    const FString &SourceString, FTextLayout &TargetTextLayout,
    TArray<ISyntaxTokenizer::FTokenizedLine> TokenizedLines) {
  // Clear existing lines
  TargetTextLayout.ClearLines();

  TArray<FTextLayout::FNewLineData> LinesToAdd;
  LinesToAdd.Reserve(TokenizedLines.Num());

  for (const ISyntaxTokenizer::FTokenizedLine &TokenizedLine : TokenizedLines) {
    TSharedRef<FString> LineText = MakeShared<FString>(SourceString.Mid(
        TokenizedLine.Range.BeginIndex, TokenizedLine.Range.Len()));
    TArray<TSharedRef<IRun>> Runs;

    int32 RunStart = 0; // Relative to line start

    for (const ISyntaxTokenizer::FToken &Token : TokenizedLine.Tokens) {
      // Calculate relative offsets
      int32 TokenStart =
          Token.Range.BeginIndex - TokenizedLine.Range.BeginIndex;
      int32 TokenLen = Token.Range.Len();

      // Add gap if needed (whitespace between tokens)
      if (TokenStart > RunStart) {
        Runs.Add(FSlateTextRun::Create(FRunInfo(), LineText, NormalTextStyle,
                                       FTextRange(RunStart, TokenStart)));
      }

      // Get style for token type
      const FTextBlockStyle *RunStyle = &NormalTextStyle;
      ECppTokenType TokenType = static_cast<ECppTokenType>(Token.Type);

      switch (TokenType) {
      case ECppTokenType::Comment:
        RunStyle = &CommentTextStyle;
        break;
      case ECppTokenType::String:
        RunStyle = &StringTextStyle;
        break;
      case ECppTokenType::Keyword:
        RunStyle = &KeywordTextStyle;
        break;
      case ECppTokenType::ControlFlow:
        RunStyle = &ControlFlowStyle;
        break;
      case ECppTokenType::Type:
        RunStyle = &TypeTextStyle;
        break;
      case ECppTokenType::UnrealMacro:
        RunStyle = &MacroTextStyle;
        break;
      case ECppTokenType::FunctionCall:
        RunStyle = &FunctionCallStyle;
        break;
      case ECppTokenType::ClassName:
        RunStyle = &ClassNameStyle;
        break;
      case ECppTokenType::Namespace:
        RunStyle = &NamespaceStyle;
        break;
      case ECppTokenType::Number:
        RunStyle = &NumberTextStyle;
        break;
      case ECppTokenType::Operator:
        RunStyle = &OperatorTextStyle;
        break;
      case ECppTokenType::Punctuation:
        RunStyle = &PunctuationStyle;
        break;
      case ECppTokenType::PreProcessor:
        RunStyle = &PreProcessorStyle;
        break;
      case ECppTokenType::IncludePath:
        RunStyle = &IncludePathStyle;
        break;
      case ECppTokenType::MemberAccess:
        RunStyle = &OperatorTextStyle;
        break;
      default:
        break;
      }

      // Add token run
      Runs.Add(
          FSlateTextRun::Create(FRunInfo(), LineText, *RunStyle,
                                FTextRange(TokenStart, TokenStart + TokenLen)));

      RunStart = TokenStart + TokenLen;
    }

    // Add final run for remaining content (trailing whitespace)
    if (RunStart < LineText->Len()) {
      Runs.Add(FSlateTextRun::Create(FRunInfo(), LineText, NormalTextStyle,
                                     FTextRange(RunStart, LineText->Len())));
    }

    // Handle empty lines
    if (LineText->IsEmpty()) {
      Runs.Add(FSlateTextRun::Create(FRunInfo(), LineText, NormalTextStyle,
                                     FTextRange(0, 0)));
    }

    LinesToAdd.Add(FTextLayout::FNewLineData(LineText, Runs));
  }

  TargetTextLayout.AddLines(LinesToAdd);
}
