//
//  MacMenu_mm.m
//  Micropolis-SDL2PP
//
//  Created by Toto on 25/05/2025.
//

#import "MacMenu.h"

#import "main.h"
#import "w_sound.h"
#import "Sprite.h"
#import "s_disast.h"
#include "gameOptions.h"
#include "MiniMapWindow.h"

NSMenuItem *autoBudgetMenuItem;
NSMenuItem *autoBulldozeMenuItem;
NSMenuItem *disastersMenuItem;
NSMenuItem *soundMenuItem;
NSMenuItem *musicMenuItem;
NSMenuItem *animationMenuItem;
NSMenuItem *messagesMenuItem;
NSMenuItem *noticesMenuItem;

NSMenuItem *monsterMenuItem;
NSMenuItem *fireMenuItem;
NSMenuItem *floodMenuItem;
NSMenuItem *meltdownMenuItem;
NSMenuItem *tornadoMenuItem;
NSMenuItem *earthquakeMenuItem;

NSMenuItem *minimapMenuItem;
NSMenuItem *budgetMenuItem;
NSMenuItem *evaluationMenuItem;
NSMenuItem *graphMenuItem;

NSMutableArray *speedMenuItems;
NSMutableArray *zoomMenuItems;

@implementation MacMenu

- (void) buildMenu {
	
	NSMenu *menuBar = [NSMenu new];
	
	
	NSMenuItem *appMenuItem = [NSMenuItem new];
	NSMenu *appMenu = [NSMenu new];
	
	[[appMenu addItemWithTitle:@"Home Screen" action:@selector(home:) keyEquivalent:@""] setTarget:self];
	[[appMenu addItemWithTitle:@"Save City" action:@selector(saveCity:) keyEquivalent:@"s"] setTarget:self];
	[[appMenu addItemWithTitle:@"Save City as..." action:@selector(saveCityAs:) keyEquivalent:@"v"] setTarget:self];
	[[appMenu addItemWithTitle:@"About..." action:@selector(about:) keyEquivalent:@""] setTarget:self];
	[appMenu addItemWithTitle:@"Quit Playing!" action:@selector(terminate:) keyEquivalent:@"q"];
	[appMenuItem setSubmenu:appMenu];
	[menuBar addItem:appMenuItem];
	
	
	NSMenuItem *optionsMenuItem = [NSMenuItem new];
	NSMenu *optionsMenu = [[NSMenu alloc] initWithTitle:@"Options"];
	
	autoBudgetMenuItem = [optionsMenu addItemWithTitle:@"Auto Budget" action:@selector(toggleAction:) keyEquivalent:@""];
	[autoBudgetMenuItem setTarget:self];
	[autoBudgetMenuItem setState:NSControlStateValueOn];
	
	autoBulldozeMenuItem = [optionsMenu addItemWithTitle:@"Auto Bulldoze" action:@selector(toggleAction:) keyEquivalent:@""];
	[autoBulldozeMenuItem setTarget:self];
	[autoBulldozeMenuItem setState:NSControlStateValueOn];
	
	disastersMenuItem = [optionsMenu addItemWithTitle:@"Disasters" action:@selector(toggleAction:) keyEquivalent:@""];
	[disastersMenuItem setTarget:self];
	[disastersMenuItem setState:NSControlStateValueOn];
	
	soundMenuItem = [optionsMenu addItemWithTitle:@"PlaySounds" action:@selector(toggleAction:) keyEquivalent:@""];
	[soundMenuItem setTarget:self];
	[soundMenuItem setState:NSControlStateValueOn];
	
	musicMenuItem = [optionsMenu addItemWithTitle:@"Play Music" action:@selector(toggleAction:) keyEquivalent:@""];
	[musicMenuItem setTarget:self];
	[musicMenuItem setState:NSControlStateValueOn];
	
	animationMenuItem = [optionsMenu addItemWithTitle:@"Animation" action:@selector(toggleAction:) keyEquivalent:@""];
	[animationMenuItem setTarget:self];
	[animationMenuItem setState:NSControlStateValueOn];
	
	noticesMenuItem = [optionsMenu addItemWithTitle:@"Notices" action:@selector(toggleAction:) keyEquivalent:@""];
	[noticesMenuItem setTarget:self];
	[noticesMenuItem setState:NSControlStateValueOn];
	
	[optionsMenuItem setSubmenu: optionsMenu];
	[menuBar addItem: optionsMenuItem];
	
	
	NSMenuItem *disastersMenuItem = [NSMenuItem new];
	NSMenu *disastersMenu = [[NSMenu alloc] initWithTitle:@"Disasters"];
	
	monsterMenuItem = [disastersMenu addItemWithTitle:@"Monster" action:@selector(monsterAction:) keyEquivalent:@""];
	[monsterMenuItem setTarget:self];
	
	fireMenuItem = [disastersMenu addItemWithTitle:@"Fire" action:@selector(fireAction:) keyEquivalent:@""];
	[fireMenuItem setTarget:self];
	
	floodMenuItem = [disastersMenu addItemWithTitle:@"Flood" action:@selector(floodAction:) keyEquivalent:@""];
	[floodMenuItem setTarget:self];
	
	meltdownMenuItem = [disastersMenu addItemWithTitle:@"Meltdown" action:@selector(meltdownAction:) keyEquivalent:@""];
	[meltdownMenuItem setTarget:self];
	
	tornadoMenuItem = [disastersMenu addItemWithTitle:@"Tornado" action:@selector(tornadoAction:) keyEquivalent:@""];
	[tornadoMenuItem setTarget:self];
	
	earthquakeMenuItem = [disastersMenu addItemWithTitle:@"Earthquake" action:@selector(earthquakeAction:) keyEquivalent:@""];
	[earthquakeMenuItem setTarget:self];
	
	[disastersMenuItem setSubmenu: disastersMenu];
	[menuBar addItem: disastersMenuItem];
	
	
	NSArray *speeds = @[@"Super Fast", @"Fast", @"Normal", @"Slow", @"Paused"];
	speedMenuItems = [[NSMutableArray alloc] init];
	NSMenuItem *priorityMenuItem = [NSMenuItem new];
	NSMenu *priorityMenu = [[NSMenu alloc] initWithTitle:@"Priority"];
	
	for (NSString *speed in speeds) {
		NSMenuItem *item = [priorityMenu addItemWithTitle:speed action:@selector(setSpeedAction:) keyEquivalent:@""];
		[item setTarget:self];
		[item setState:[speed isEqualToString:@"Normal"]];
		[speedMenuItems addObject:item];
	}
	
	[priorityMenuItem setSubmenu: priorityMenu];
	[menuBar addItem: priorityMenuItem];
	
	
	NSArray *zooms = @[@"Zoom In", @"Zoom out"];
	NSArray *keys = @[@"+", @"-"];
	zoomMenuItems = [[NSMutableArray alloc] init];
	NSMenuItem *zoomMenuItem = [NSMenuItem new];
	NSMenu *zoomMenu = [[NSMenu alloc] initWithTitle:@"Zoom"];
	int i = 0;
	for (NSString *zoom in zooms) {
		NSMenuItem *item = [zoomMenu addItemWithTitle:zoom action:@selector(setZoomAction:) keyEquivalent:keys[i++]];
		[item setTarget:self];
		[zoomMenuItems addObject:item];
	}
	
	[zoomMenuItem setSubmenu: zoomMenu];
	[menuBar addItem: zoomMenuItem];
	
	
	NSMenuItem *windowsMenuItem = [NSMenuItem new];
	NSMenu *windowsMenu = [[NSMenu alloc] initWithTitle:@"Windows"];
	
	minimapMenuItem = [windowsMenu addItemWithTitle:@"Mini map" action:@selector(toggleAction:) keyEquivalent:@""];
	[minimapMenuItem setTarget:self];
	[minimapMenuItem setState:NSControlStateValueOn];
	
	messagesMenuItem = [windowsMenu addItemWithTitle:@"Messages" action:@selector(toggleAction:) keyEquivalent:@""];
	[messagesMenuItem setTarget:self];
	[messagesMenuItem setState:NSControlStateValueOn];
	
	budgetMenuItem = [windowsMenu addItemWithTitle:@"Budget" action:@selector(toggleAction:) keyEquivalent:@""];
	[budgetMenuItem setTarget:self];
	[budgetMenuItem setState:NSControlStateValueOff];
	
	evaluationMenuItem = [windowsMenu addItemWithTitle:@"Evaluation" action:@selector(toggleAction:) keyEquivalent:@""];
	[evaluationMenuItem setTarget:self];
	[evaluationMenuItem setState:NSControlStateValueOff];
	
	graphMenuItem = [windowsMenu addItemWithTitle:@"Graph" action:@selector(toggleAction:) keyEquivalent:@""];
	[graphMenuItem setTarget:self];
	[graphMenuItem setState:NSControlStateValueOff];
	
	[windowsMenuItem setSubmenu: windowsMenu];
	[menuBar addItem: windowsMenuItem];
	
	
	[[NSApplication sharedApplication] setMainMenu:menuBar];
}

- (id) init {
	if ( self = [super init] ) {
		[self buildMenu];
	}
	return self;
}

- (void) menuAction: (id)sender {
	NSLog(@"%@", sender);
	
	NSMenuItem *item = sender;
}

- (void) home: (id)sender {
	showWindowAndBringToFront(*appWindows.home);
	showWindowAndBringToFront(*appWindows.homeMiniMap);
}

- (void) about: (id)sender {
    showAboutWindow();
}

- (void) monsterAction: (id)sender {
    generateMonster();
}

- (void) fireAction: (id)sender {
    MakeFire();
}

- (void) floodAction: (id)sender {
    MakeFlood();
}

- (void) meltdownAction: (id)sender {
    MakeMeltdown();
}

- (void) tornadoAction: (id)sender {
    generateTornado();
}

- (void) earthquakeAction: (id)sender {
    MakeEarthquake();
}

- (void) setZoomAction: (id)sender {
    int newRenderTileSize = 32;
    
    if (sender == zoomMenuItems[0])
    {
        newRenderTileSize = MIN(64, MAX(8, renderTileSize + 1));
        
    } else if (sender == zoomMenuItems[1]) {
        newRenderTileSize = MIN(64, MAX(8, renderTileSize - 1));
    }
    
    if (newRenderTileSize != renderTileSize)
    {
        renderTileSize = newRenderTileSize;
        setRenderTileSize();
    }

}

- (void) setSpeedAction: (id)sender {
    int i = 0;
    for (NSMenuItem *item in speedMenuItems) {
        [item setState:item == sender];
        if (item == sender) {
            setSpeed((SimulationSpeed)(4 - i));
        }
        i++;
    }
}

- (void) toggleAction: (id)sender {
    NSLog(@"%@", sender);
    
    NSMenuItem *item = sender;
    
    item.state = !item.state;
    
	gameOptions.mAutoBudget = autoBudgetMenuItem.state;
	gameOptions.mAutoBulldoze = autoBulldozeMenuItem.state;
	gameOptions.mDisasters = disastersMenuItem.state;
	gameOptions.mPlaySounds = soundMenuItem.state;
	gameOptions.mPlayMusic = musicMenuItem.state;
	gameOptions.mAnimation = animationMenuItem.state;
	gameOptions.mNotices = noticesMenuItem.state;
	
    if (sender == minimapMenuItem)
    {
        toggleMiniMapVisibility();
    }
    
	if (sender == messagesMenuItem)
	{
		showMessagesWindow();
	}
	
    if (sender == budgetMenuItem)
    {
        showBudgetWindow();
    }
    
    if (sender == evaluationMenuItem)
    {
        showEvaluationWindow();
    }
    
    if (sender == graphMenuItem)
    {
        showGraphWindow();
    }
	
	optionsChanged();
}

- (void) saveCity: (id)sender {
    saveGame();
}

- (void) saveCityAs: (id)sender {
	if (appWindows.saveAs)
	{
		appWindows.saveAs->setCityName(cityProperties.CityName());
		// showWindowAndBringToFront() re-centers the window for us
		// (since saveAs isn't status/messages), so no explicit centerWindow call.
		showWindowAndBringToFront(*appWindows.saveAs);
	}
}

@end

