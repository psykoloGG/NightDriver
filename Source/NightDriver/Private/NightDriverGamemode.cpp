#include "NightDriverGamemode.h"

#include "CarPawn.h"

ANightDriverGamemode::ANightDriverGamemode()
{
	DefaultPawnClass = ACarPawn::StaticClass();
}
