#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "YJPlayerControllerBase.generated.h"

class UYJBlockController;
class UYJSingleModeWindow;

/**
 * 
 */
UCLASS()
class TETRIS_API AYJPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:

protected:
	/** Allows the PlayerController to set up custom input bindings. */
	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;
	
protected: // Input
	// Test
	virtual void OnChangeBlock();

	// Move
	virtual void OnMoveLeft_Pressed(FKey Key);
	virtual void OnMoveRight_Pressed(FKey Key);
	virtual void OnMoveDown_Pressed(FKey Key);

	virtual void OnMoveLeft_Released(FKey Key);
	virtual void OnMoveRight_Released(FKey Key);
	virtual void OnMoveDown_Released(FKey Key);

	// Action
	virtual void OnRotateClockWise(FKey Key);
	virtual void OnRotateCounterClockWise(FKey Key);
	virtual void OnDrop(FKey Key);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UYJSingleModeWindow> SingleModeWindowClass;

	UPROPERTY()
	UYJSingleModeWindow* SingleModeWindow;

	UPROPERTY()
	UYJBlockController* BlockController;

	UPROPERTY(EditDefaultsOnly)
	int32 Row = 20;

	UPROPERTY(EditDefaultsOnly)
	int32 Column = 10;
};
