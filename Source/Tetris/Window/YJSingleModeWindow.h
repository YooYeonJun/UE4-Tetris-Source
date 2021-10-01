#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YJSingleModeWindow.generated.h"

class UYJBlockGridWidget;

/**
 * 
 */
UCLASS()
class TETRIS_API UYJSingleModeWindow : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UYJBlockGridWidget* GetBlockGridWidget() const { return BlockGridWidget; }

private:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, BindWidget))
	UYJBlockGridWidget* BlockGridWidget = nullptr;
};
