//
//  MacMenu_mm.h
//  Micropolis-SDL2PP
//
//  Created by Toto on 25/05/2025.
//

#pragma once

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

extern NSMenuItem *autoBudgetMenuItem;
extern NSMenuItem *autoBulldozeMenuItem;
extern NSMenuItem *disastersMenuItem;
extern NSMenuItem *soundMenuItem;
extern NSMenuItem *animationMenuItem;
extern NSMenuItem *messagesMenuItem;
extern NSMenuItem *noticesMenuItem;

extern NSMenuItem *monsterMenuItem;
extern NSMenuItem *fireMenuItem;
extern NSMenuItem *floodMenuItem;
extern NSMenuItem *meltdownMenuItem;
extern NSMenuItem *tornadoMenuItem;
extern NSMenuItem *earthquakeMenuItem;

extern NSMutableArray *speedMenuItems;

extern NSMenuItem *minimapMenuItem;
extern NSMenuItem *budgetMenuItem;
extern NSMenuItem *evaluationMenuItem;
extern NSMenuItem *graphMenuItem;

@interface MacMenu : NSObject
- (void) about: (id)sender;
@end

