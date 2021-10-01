#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YJBlockWidget.generated.h"

class UImage;
class UUniformGridPanel;
class UTextBlock;

/**
 * 블록 위젯
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class TETRIS_API UYJBlockWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetColor(FLinearColor InColor);
	void SetSize(FVector2D InSize);

protected:

private:
	bool CheckValid() const;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, BindWidget))
	UImage* Image_Block = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * 블록그리드 위젯
 * 좌상 (0,0) / 우하(Column, Row)
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class TETRIS_API UYJBlockGridWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(int32 InRow, int32 InColumn, const FLinearColor& InColorBG);
	void CreateGrid(int32 NewRow, int32 NewColumn, const FLinearColor& NewColorBG);

	UFUNCTION(CallInEditor, BlueprintCallable)
	void K2_CreateGrid();

	void Refresh(int32 RowIndex, int32 ColumnIndex, const FLinearColor& NewColor);

	void SetBlockInfo(FText BlockInfo);

protected:
	virtual void NativePreConstruct() override;

private:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, BindWidget))
	UTextBlock* TextBlock_BlockInfo = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, BindWidget))
	UUniformGridPanel* UniformGridPanel_Blocks = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TSubclassOf<UYJBlockWidget> BlockWidgetClass;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, UIMin = 0))
	int32 Row = 10;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, UIMin = 0))
	int32 Column = 10;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, UIMin = 0))
	int32 BlockSize = 10;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FLinearColor ColorBG;
};
