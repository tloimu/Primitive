#pragma once

#include "Interactable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
    GENERATED_BODY()
};

class IInteractable
{    
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void Interact();
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void Hit();
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void Consume();
    //UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void CombineWith(IInteractable* CombineWith);

    /*
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent) bool CanInteract();
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent) bool CanHit();
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent) bool CanConsume();
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent) bool CanCombineWith(IInteractable* CombineWith);
    */
};
