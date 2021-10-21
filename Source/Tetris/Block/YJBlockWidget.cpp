#include "YJBlockWidget.h"
#include "../Tetris.h"

#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"

#include "Blueprint/WidgetTree.h"
#include "Blueprint/WidgetLayoutLibrary.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// UYJBlockWidget

void UYJBlockWidget::SetColor(FLinearColor InColor)
{
	if (!CheckValid())
	{
		return;
	}
	Image_Block->SetColorAndOpacity(InColor);
}

void UYJBlockWidget::SetSize(FVector2D InSize)
{
	if (!CheckValid())
	{
		return;
	}
	Image_Block->SetBrushSize(InSize);
}

bool UYJBlockWidget::CheckValid() const
{
	if (!IsValid(Image_Block))
	{
		UE_LOG(LogYJ, Log, TEXT("Image_Block is not valid"));
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// UYJBlockGridWidget

void UYJBlockGridWidget::Init(int32 InRow, int32 InColumn, const FLinearColor& InColorBG)
{
	CreateGrid(InRow, InColumn, InColorBG);
}

void UYJBlockGridWidget::K2_CreateGrid()
{
	CreateGrid(Row, Column, ColorBG);
}

void UYJBlockGridWidget::Refresh(int32 RowIndex, int32 ColumnIndex, const FLinearColor& NewColor)
{
	if (auto* Widget = Cast<UYJBlockWidget>(UniformGridPanel_Blocks->GetChildAt(RowIndex * Column + ColumnIndex)))
	{
		Widget->SetColor(NewColor);
	}
}

void UYJBlockGridWidget::SetBlockInfo(FText BlockInfo)
{
	if (TextBlock_BlockInfo)
	{
		TextBlock_BlockInfo->SetText(BlockInfo);
	}
}

void UYJBlockGridWidget::CreateGrid(int32 NewRow, int32 NewColumn, const FLinearColor& NewColorBG)
{
	if (!BlockWidgetClass.Get())
	{
		UE_LOG(LogYJ, Warning, TEXT("BlockWidget is not valid"));
		return;
	}

	if (!UniformGridPanel_Blocks)
	{
		UE_LOG(LogYJ, Warning, TEXT("UniformGridPanel_Blocks is not valid"));
		return;
	}

	Row = NewRow;
	Column = NewColumn;
	ColorBG = NewColorBG;

	UniformGridPanel_Blocks->ClearChildren();

	for (int32 RowIndex = 0; RowIndex < Row; RowIndex++)
	{
		for (int32 ColumnIndex = 0; ColumnIndex < Column; ColumnIndex++)
		{
			const FName BlockName = *FString::Printf(TEXT("BlockPartsWidget_%d_%d"), RowIndex, ColumnIndex );
			
			UYJBlockWidget* BlockWidget = CreateWidget<UYJBlockWidget>(this, BlockWidgetClass, BlockName);
			BlockWidget->SetVisibility(ESlateVisibility::Visible);
			BlockWidget->SetColor(ColorBG);
			BlockWidget->SetSize(FVector2D::UnitVector * BlockSize);
			UniformGridPanel_Blocks->AddChildToUniformGrid(BlockWidget, RowIndex, ColumnIndex);
		}
	}

	UE_LOG(LogYJ, Log, TEXT("CreateGrid Row %d, Column %d Color %s"), Row, Column, *ColorBG.ToString());
}

void UYJBlockGridWidget::NativePreConstruct()
{
	if (IsDesignTime())
	{
		K2_CreateGrid();
	}
	Super::NativePreConstruct();
}

