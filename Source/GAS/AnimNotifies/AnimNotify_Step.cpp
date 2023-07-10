// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_Step.h"
#include "GASCharacter.h"
#include "ActorComponents/FootstepsComponent.h"

void UAnimNotify_Step::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	
	check(MeshComp);

	if(const AGASCharacter* Character = Cast<AGASCharacter>(MeshComp ? MeshComp->GetOwner() : nullptr))
	{
		if(UFootstepsComponent* FootstepsComponent = Character->GetFootstepsComponent())
		{
			FootstepsComponent->HandleFootstep(Foot);
		}
	}
}