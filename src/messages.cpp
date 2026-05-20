/* messages
 *
 * Micropolis, Unix Version.  This game was released for the Unix platform
 * in or about 1990 and has been modified for inclusion in the One Laptop
 * Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
 * you need assistance with this program, you may contact:
 *   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 * 
 *             ADDITIONAL TERMS per GNU GPL Section 7
 * 
 * No trademark or publicity rights are granted.  This license does NOT
 * give you any right, title or interest in the trademark SimCity or any
 * other Electronic Arts trademark.  You may not distribute any
 * modification of this program using the trademark SimCity or claim any
 * affliation or association with Electronic Arts Inc. or its employees.
 * 
 * Any propagation or conveyance of this program must include this
 * copyright notice and these terms.
 * 
 * If you convey this program (or any modifications of it) and assume
 * contractual liability for the program to recipients of it, you agree
 * to indemnify Electronic Arts for any liability that those contractual
 * assumptions impose on Electronic Arts.
 * 
 * You may not misrepresent the origins of this program; modified
 * versions of the program must be marked as such and not identified as
 * the original program.
 * 
 * This disclaimer supplements the one included in the General Public
 * License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
 * PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
 * OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
 * SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
 * DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
 * INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
 * USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
 * MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
 * WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
 * CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
 * ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
 * JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
 * WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
 * CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
 * NOT APPLY TO YOU.
 */

#include "main.h"

struct msgpr popMsgs[] = {
{ "TRAFFIC WARNING- Traffic in this city is expanding. The commuters are getting militant. Highway shootings are on the rise. Either build more roads and rails or get a bulletproof limo.", -12 },

{ "CRIME ALERT- Crime in your city is out of hand. Angry mobs are looting and vandalizing the central city. The president will send in the national guard soon if you cannot control the problem.", -11 },

{ "NUCLEAR MELTDOWN- A nuclear meltdown has occurred at your power plant. You are advised to avoid the area until the radioactive isotopes decay. Many  generations will confront this problem before it goes away so don't hold your breath.", -43 },

{ "YOU'RE A WINNER- Your mayorial skill and city planning expertise have earned you the KEY TO THE CITY. Local residents will erect monuments to your glory and name their first-born children after you. Why not run for governor?", -100 },

{ "IMPEACHMENT NOTICE- The entire population of this city has finally had enough of your inept planning  and incompetant management. An angry mob - lead by your mother - has been spotted in the vicinity of city hall. You should seriously consider taking an extended vacation - NOW. (Or read the manual and try again).", -200 },

{ "MONSTER ATTACK- A large reptilian creature has been spotted in the water. It seems to be attracted to  areas of high pollution. There is a trail of destruction wherever it  goes.  As a last resort try calling (Kanji)  Other than that wait till it leaves then rebuild from the rubble.", -21 },

{ "POLLUTION ALERT- Pollution in your city has exceeded the maximum allowable amounts established by the Micropolis pollution agency. You are running the risk of grave ecological consequences. Either clean up your act or open a gas mask concession at city hall.", -10 },

{ "MEGALOPOLIS- Congratulations, you have reached the highest category of urban development, the megalopolis. If you manage to reach this level send us email at micropolis@laptop.org or send us a copy of your city. We  might do something interesting with it.", -39 },

{ "METROPOLIS- Your capital city has now achieved the status of metropolis. The current population is 100,000. With your planning skills, you should consider running for governor, or maybe VLSI design.", -38 },

{ "CAPITAL- Your current city has become a capital.  The current population here is 50,000. Your political future looks bright.", -37 },

{ "CITY- Your town has grown into a full sized city, with a current population of 10,000. Keep up the good work!", -36 },

{ "TOWN- Congratulations, your village has grown to town status. You now have 2,000 citizens.", -35 },


{ "Detroit, MI. 1972- By 1970, competition from overseas and other economic factors pushed the once automobile capital of the world into recession.  Plummeting land values and unemployment then increased crime in the inner-city to chronic levels.  You have 10 years to reduce crime and rebuild the industrial base of the city. ", -6 },

{ "Rio de Janiero, Brazil 2047- By the mid-21st century, the greenhouse effect increased global temperatures about 4 degrees F. Polar icecaps melted and raised sea levels worldwide. Coastal areas were devastated by flood and erosion. You have 10 years to turn this swamp back into a city again.", -8 },

{ "Tokyo, Japan 1957- A large reptilian creature has been spotted heading for Tokyo bay. It seems to be attracted to the heavy levels of industrial pollution.  Try to control the fires, then rebuild the industrial center. You have five years.", -5 },

{ "Hamburg, Germany 1944- Allied fire-bombing of German cities in WWII caused tremendous damage and loss of life. People living in the inner cities were at greatest risk. You must control the firestorms during the bombing and then rebuild the city after the war. You have five years.", -3 },

{ "San Francisco, CA. 1906- Damage from the earthquake was minor compared to that of the ensuing fires, which took days to control. 1500 people died.  Controlling the fires should be your initial concern. Then clear the rubble and start rebuilding. You have five years.", -2 },

{ "Dullsville, USA 1900- Things haven't changed much around here in the last hundred years or so and the residents are beginning to get bored. They think Dullsville could be the next great city with the right leader. It is your job to attract new growth and development, turning Dullsville into a Metropolis within 30 years.", -1 },

{ "Boston, MA. 2010- A major meltdown is about to occur at one of the new downtown nuclear reactors. The area in the vicinity of the reactor will be severely  contaminated by radiation, forcing you to restructure the city around it. You have 5 years to get the situation under control.", -7 },

{ "Bern, Switzerland 1965- The roads here are becoming more congested every day, and the residents are upset. They demand that you do something about it. Some have suggested a mass transit system as the answer, but this would require major rezoning in the downtown area. You have ten years.", -4 },

{NULL, 0}
};
