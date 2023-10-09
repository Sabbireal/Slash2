#pragma once

#include "DrawDebugHelpers.h"

#define	DRAW_SPHERE(Location) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 24, FColor::Blue, true);
#define DRAW_LINE(StartLocation, EndLocation) if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, true);
#define	DRAW_POINT(Location) if(GetWorld()) DrawDebugPoint(GetWorld(), Location, 10.f, FColor::Blue, true);

#define DRAW_VECTOR(StartLocation, EndLocation) if(GetWorld()) \
	{ \
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, true); \
		DrawDebugPoint(GetWorld(), EndLocation, 10.f, FColor::Blue, true); \
	}

#define	DRAW_SPHERE_SingleFrame(Location) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 24, FColor::Blue, false);
#define DRAW_LINEE_SingleFrame(StartLocation, EndLocation) if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, false);
#define	DRAW_POINTE_SingleFrame(Location) if(GetWorld()) DrawDebugPoint(GetWorld(), Location, 10.f, FColor::Blue, false);

#define DRAW_VECTOR_SingleFrame(StartLocation, EndLocation) if(GetWorld()) \
	{ \
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, false); \
		DrawDebugPoint(GetWorld(), EndLocation, 10.f, FColor::Blue, false); \
	}
