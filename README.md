
# CursedRL: Tower of Malevolent Divinity
<img width="1900" height="1011" alt="gameCursedRL" src="https://github.com/user-attachments/assets/8767d51c-75e4-46ca-87de-20bbbcaa28fb" />

A small traditional roguelike foundation for Windows and CLion, written in
C++20 and rendered with Dear ImGui. It includes a connected grid dungeon,
eight-direction movement, line-of-sight field of view, remembered tiles,
enemies, turn-based melee combat, a message log, and a compact status panel.
It opens on a main menu and includes character selection with four clans:
Gojo, Zenin, Jogo, and Nanami.
The complete controls panel can be opened from either the main menu or the
gameplay Escape menu.

Dungeon floors use a 16-variant top-down wooden tileset inspired by warm
cypress and cedar plank floors from Heian-era Japanese palace architecture.
The stable per-cell variations darken when explored tiles leave the player's
current field of view.

Walls use a complementary 16-variant Japanese castle stone tileset inspired by
dark ishigaki masonry, with irregular fieldstones, fitted foundation blocks,
and subdued mortar. A dark filter is applied to every wall, becoming heavier
outside the current field of view. Walls bordering hallways use one consistent
style. Walls bordering rooms use a designated room style 75% of the time and
draw from the other stone variants for the remaining 25%.

Up and down stairs use dedicated wooden stair tiles with dark stone edging.
Two variants of each direction alternate by dungeon level, darken outside the
current field of view, and retain the `<` and `>` overlays for readability.

Starting a new game first opens a name-entry screen. The entered given name is
combined with the selected clan as a surname (for example, `Akira Gojo`). The
full name appears in combat messages, player inspection, the sidebar, and the
Game Over summary.

Clan names are color-coded throughout selection and character statistics:
Gojo is blue, Zenin is purple, Jogo is red, and Nanami is green.
Character creation divides clan selection into two tabs: **Sorcerers** contains
Gojo, Zenin, and Nanami, while **Cursed Spirits** currently contains Jogo.
Each clan uses a full-width selection card, with longer Cursed Technique
descriptions wrapping within the clan panel.
The clan-selection screen also shows each clan's Cursed Technique, its initial
Lapse effect, base damage or summon statistics, and Cursed Energy cost.
The player uses a distinct transparent pixel-art map sprite for each clan,
themed in the same blue, purple, red, or green clan color. Player sprites scale
with map zoom; the traditional `@` remains as a fallback if the atlas cannot
be loaded. Player inspection uses a separate detailed 2×2 atlas derived from
those gameplay designs, displayed as a large centered portrait above the stats.

Enemies use transparent pixel-art sprites selected from their combat type:
separate Bite, Claws, and Stinger Insect Curses, a weathered Ronin, and a
violet-corrupted Cursed Host. Their original glyphs remain as texture-loading
fallbacks, while targeting and status outlines continue to render above them.
Inspecting a hostile displays its atlas sprite as a large portrait. Inspection
stats follow the main palette: Health red, Attack green, Cursed Energy purple,
and CE Generation pink.

Arrow keys are reserved for gameplay movement and targeting. They do not
navigate or alter menu controls; menus are operated with the mouse.

Press `Escape` during normal gameplay to open the Pause Menu. It provides
Resume, Sound, New Game, Main Menu, and Quit options. Sound opens a separate
gameplay-music volume panel. The New Game control is located
here instead of in the sidebar. When targeting or another modal is open,
Escape cancels or closes that interface before opening the Pause Menu.

Player statistics use a consistent palette: Health is red, Attack is green,
Cursed Energy is purple, Efficiency is teal, and Cursed Energy Generation is
pink. Attack changes to purple when the current melee strike can affect curses.
The sidebar also displays the current total damage of the clan's primary
Cursed Technique after CT Power, Technique Output, and relevant multipliers.
All sidebar labels and descriptions wrap to the available panel width.

Character creation begins with **5 Birth Points** shared by starting stats and
traits:

The player begins with 30 Health before Birth Point bonuses.

- Health: 1 point adds 10 maximum HP
- Attack: 1 point adds 1 base attack
- Cursed Energy: 1 point adds 10 maximum energy
- Cursed Energy Efficiency: 1 point adds 5%
- Cursed Energy Generation: 2 points add 1 energy regained per turn
- Cursed Technique Power: 1 point adds 2 technique damage
- Close Combat Ability: starts at 1; 3 points add one melee attack to every
  melee action
- Serrated Cursed Energy: costs 3 points; whenever defense reinforcement blocks
  damage, it reflects 1 damage per reinforcement point actually used back to
  the attacker
- Reversed Cursed Technique: Healing: unavailable during character creation;
  human clans can learn it from Grade Advancement starting at Grade 2
- Cursed Healing: Jogo clan characters count as Cursed Spirits and begin with
  this innate alternative at no Birth Point cost. It uses the same energy and
  healing values as Reversed Cursed Technique: Healing.
- No Innate Technique: a negative trait that adds 5 Birth Points, but prevents
  use of the selected clan's Cursed Technique. Cursed Energy Reinforcement
  remains fully available.

Traits are organized into color-coded **Positive Traits** and **Negative
Traits** tabs. Jogo receives Cursed Healing automatically, while human clans
must wait until Grade 2 to learn Reversed Cursed Technique: Healing through
Grade Advancement. There is no separate Birth Abilities section.

**Simple Domain** costs 3 Birth Points as a positive trait. If it was not
selected at birth, it can be learned during Grade Advancement for 10 run points.
Press `X` during play to spend 10 base Cursed Energy, adjusted by Cursed Energy
Efficiency, and arm it.
The next direct attack against the player is completely blocked, then its
attacker is countered for 1.5 times the player's base Attack. Arming the domain
uses a turn, and it remains armed until an attack triggers it. A pulsing blue
aura surrounds the player while Simple Domain is armed.

The message log appears in a dedicated **Messages** panel beneath the dungeon,
outside the map area like the right-hand sidebar. Drag the horizontal divider
to resize its height. It never overlaps the map or sidebar, and message
contents scroll independently. Messages use distinct colors for damage,
healing, poison, techniques, warnings, rewards, ranks, and debug events.
Whenever the player takes damage, the message includes remaining HP: green
above 75%, yellow from 50% through 75%, and red below 50%.
Enemy damage resolution is also color-coded: Cursed Energy spent on defensive
reinforcement is indigo, damage blocked is brown, and damage dealt by the
player, their technique, counters, reflections, or Shikigami uses the player's
clan color.

Unspent Birth Points are allowed. Allocations become the starting values when
**Begin adventure** is selected.
After character generation, a separate screen asks the player to choose one
starting item: Black Training Uniform, Steel Katana, or Wrapped Cursed Tanto.
Only the selected item is carried at the start of the run, and it begins
equipped. Returning to character generation preserves the current clan and
Birth Point selections.
The player starts each game with 100/100 Cursed Energy, a resource reserved for
abilities added as the game grows.

The player also starts with **100% Cursed Energy Efficiency**. Every energy cost
is calculated as `ceil(base cost × 100 / efficiency)`, with a minimum final
cost of 1. At 200% efficiency an ability costs half its base amount; at 50%
efficiency it costs twice as much. The sidebar always displays the current
efficiency and adjusted costs.

Every energy-bearing entity has **Cursed Energy Generation**, the amount of
Cursed Energy regained after each completed turn. The player, Insect Curses,
and Ronin begin at 1 per turn. The player can improve it during character
creation or Grade Advancement.

Every clan can use **Cursed Energy Reinforcement**. The sidebar accepts separate
attack and defense values from 0 to 20. Attack reinforcement adds that much
damage to each player strike; defense reinforcement reduces incoming damage.
Each point actually applied costs 5 Cursed Energy. When the configured value is
not fully affordable—or more defense is configured than the incoming
damage—the game applies and charges only the useful affordable amount.

The Gojo clan begins with the **Limitless** Cursed Technique. Press `1` to use
**Lapse: Limitless**, cycle highlighted visible targets with the arrow
keys, numpad, or Tab, then press `1` or Space to confirm. Escape cancels targeting.
The ability costs 25 Cursed Energy and deals a flat 10 base damage to the
selected enemy and every enemy within one tile of it. Player Attack and attack
reinforcement do not affect this damage; Technique Output and Hand Signs still
scale it. It does not move its targets. A blue 3×3 preview shows the affected
area while targeting.
Casting creates an inward blue vacuum effect: glowing cyan motes and streaks
collapse around the selected tile, scaling with the current map zoom.

During Grade Advancement, Gojo characters can spend 10 points to learn
**Cursed Technique Neutral: Barrier Technique**. Press `2` to toggle Neutral
Limitless. While active, its glowing blue aura prevents direct attacks from
reaching the player. It costs 30 base Cursed Energy per turn, adjusted by
Cursed Energy Efficiency, and collapses before enemies act if the upkeep cannot
be paid.

At Grade 2, every clan can spend 20 points during Grade Advancement to learn
**Domain Amplification**. Press `A` to toggle it. While active, it nullifies
hostile Domain Expansion effects, lets the player move inside hostile domains,
and allows melee attacks to pass through cursed-technique defenses such as
Neutral Limitless. In exchange, the player cannot use their own Cursed
Technique, summons, healing abilities, or Cursed Tools and is restricted to
melee attacks until Domain Amplification is switched off. It costs 20 base
Cursed Energy per turn, adjusted by Cursed Energy Efficiency, and automatically
deactivates when its upkeep cannot be paid.

At Grade 2, every clan can also spend 15 points to learn **Hollow Wicker
Basket**. Press `V` to toggle this anti-domain technique. It blocks the damage
effects of hostile Domain Expansions and remains available during Technique
Burnout. While it is active, the player cannot move, attack, use items, or use
other abilities; Space or numpad 5 maintains it for another turn, and `V`
releases it. It costs 5 base Cursed Energy per turn, adjusted by Cursed Energy
Efficiency, and collapses if its upkeep cannot be paid.
A pulsing pink grid surrounds the player while Hollow Wicker Basket is active.

At Grade 2 or higher, Gojo characters can spend 25 points during Grade
Advancement to learn **Cursed Technique Reversal: Red**. Press `3`, aim a line
with the movement keys, numpad, or mouse, and confirm with `3`, Space, or numpad
5. Red costs 40 base Cursed Energy, strikes the first enemy in its line for
1.5 times the character's customized Cursed Technique damage, and repels a
surviving target up to three spaces. Walls and occupied tiles stop the
knockback early.

The Zenin clan uses **Shadow Master**. Press `1` to use **Lapse: Shadow
Master**, summoning **Gyokuken**, a Shikigami with 15 health and 5 attack.
Each wolf costs 5 Cursed Energy per turn to maintain. A maximum of two can be
active: the first is a black shadow
wolf and the second is a phantom white wolf. If one is defeated, its matching
form can be summoned again. Gyokuken follow the player, pursue enemies they can
see, attack independently, occupy dungeon tiles, and can be attacked and killed.
Their paired black-and-white sprites scale with map zoom and appear full-size in
the inspection panel. Their strikes count as cursed-technique damage, so they
can harm cursed spirits after the spirit's Cursed Energy reinforcement resolves.
The customization panel can toggle **Totality** while no Gyokuken is active.
Totality fuses both forms into one monstrous black-and-white Shikigami, reduces
the active summon maximum to 1, doubles its base statistics to 30 HP and 10
damage, and costs 10 Cursed Energy per turn to maintain. Upkeep is adjusted by
Cursed Energy Efficiency and Technique Output. A summon dissolves before acting
when its upkeep cannot be paid. It has dedicated map and inspection sprites.
Press `2` as a Zenin to unsummon all active Shikigami. Unsummoning costs no
Cursed Energy and consumes a turn when at least one summon is dismissed.
Summoning any Gyokuken produces an animated inky-shadow manifestation at its
spawn tile: a spreading black-purple pool, rising droplets, and curling wisps
that scale with the dungeon zoom.

At Grade 3, Zenin characters may spend 10 points to learn **Cursed Technique
Extension: Summon Nue**. Press `3` to spend 30 base Cursed Energy and summon
Nue beside the player. Nue seeks the nearest enemy and self-destructs in a
radius-3 purple-lightning explosion, dealing half Cursed Technique damage and
stunning surviving enemies for three turns. Stunned enemies cannot take any
actions. Only one Nue may be active at a time. Expanded Area raises the
explosion radius to 4 and multiplies the
ability's base energy cost by 1.5 before Efficiency.

The Jogo clan uses **Disaster Flames**. Press `1` to aim **Lapse: Disaster Flames**
with the arrow keys, numpad, or mouse, then press `1`, Space, or numpad 5
to confirm. The blast costs 10 Cursed Energy, travels in a straight line until
it reaches a wall, and deals
10 damage to every enemy in its path. Its path is previewed in orange and the
cast produces animated flame and ember particles.

At Grade 3 or higher, Jogo characters can spend 10 advancement points to learn
**Cursed Technique Extension: Summon Ember Insect**. Press `2` to spend 30 base
Cursed Energy and manifest the summon on an open adjacent tile. It has no
upkeep, detects and pursues the nearest enemy anywhere on the level, and
detonates on contact. The explosion covers a three-tile radius and deals the
player's customized Cursed Technique damage to every enemy in the area. It
uses a dedicated gameplay sprite and inspection portrait, followed by a radial
fire-and-ember particle explosion.

The Nanami clan uses the **Ratio** Cursed Technique. Press `1` to use **Lapse:
Ratio**, spending a base cost of 20 Cursed Energy. Activation consumes a turn
and gives every currently visible enemy one persistent Ratio stack. Striking a
marked enemy consumes one stack and guarantees a critical hit dealing 2× total
damage; attack reinforcement is added before the critical multiplier. Ratio
can be activated repeatedly to place multiple stacks on the same targets, with
one stack consumed per critical strike. Consuming a stack with a melee hit
briefly draws a thin black ratio line with three tick marks over the target.
Technique Output adds directly to the
base 1× Ratio multiplier, and Hand Signs multiplies the customized result by
1.25.

At Grade 3 or higher, Nanami characters can spend 10 advancement points to
learn **Cursed Technique Extension: Collapse**. Press `2` to spend 30 base
Cursed Energy and aim a one-wide, five-deep area in front of the player, then confirm with `2`, Space, or numpad
5. Every enemy in the area receives one Ratio stack and is immediately struck
once using the player's normal melee attack, including applicable equipment,
reinforcement, Technique Power, and Ratio critical modifiers.

### Black Flash

While below 50% maximum health, each normal player melee attack has a 2% chance
to become a **Black Flash**. It triples the combined melee and Cursed Energy
Reinforcement attack damage, treats the strike as cursed damage, and completely
restores the player's Cursed Energy. The first Black Flash landed during a run
permanently increases Cursed Energy Efficiency by 5%. A black slash outlined in
red marks the impact. It also grants **Sorcery High** for seven turns. Sorcery
High temporarily adds 25% Cursed Energy Efficiency and raises the Black Flash
chance to 50%, even if the player is no longer below half health. Further Black
Flashes do not refresh an active duration; after Sorcery High expires, another
Black Flash can grant a new seven-turn duration. Black Flash information remains hidden in the
sidebar until the player lands one for the first time. Technique-driven melee
attacks such as Collapse and Unpaid Overtime cannot trigger Black Flash.

### Cursed Technique customization

Press `T` during play to open **Cursed Technique Customization**. Technique
Output begins at 1.00× and can be adjusted in 0.25 steps up to 2.00×. Increasing
Output multiplies both the technique's power and its base Cursed Energy cost by
the selected amount before Cursed Energy Efficiency is applied.

- Gojo: multiplies Lapse: Limitless damage.
- Zenin: multiplies Gyokuken and Nue explosion damage and can toggle Totality.
  Totality allows one fused summon with 30 HP and 10 base damage instead of two
  15 HP/5 damage wolves.
- Jogo: multiplies Disaster Flames damage.
- Nanami: adds Output to Ratio's critical multiplier, producing 2.00× at
  default Output and 3.00× at maximum Output.

**Expanded Area** is a separate customization toggle. It adds one tile to
applicable radii and widens line or rectangular areas by one tile on each side.
It expands Limitless from radius 1 to 2, Disaster Flames from one to three
tiles wide, Collapse from 1x5 to 3x5, and Ember Insect and Nue explosions from
radius 3 to 4. Applicable base Cursed Energy costs are multiplied by 1.5 before
Efficiency; this raises Collapse and Nue from 30 to 45 base energy.

Hand Signs applies its 1.25× power multiplier after Technique Output. Changing
Output does not consume a turn. Characters with No Innate Technique can view
the menu but cannot customize a clan technique.

## Requirements

- Windows 10 or 11
- CLion
- A Windows C++ toolchain configured in CLion:
  - CLion's bundled MinGW toolchain is the easiest option, or
  - Visual Studio Build Tools/MSVC also works
- Git (CMake uses it to fetch GLFW and Dear ImGui once)

CLion bundles CMake and Ninja, so separate installations normally are not
needed.

## Open and run in CLion

1. Start CLion and choose **Open**.
2. Select the project folder containing `CMakeLists.txt`.
3. If prompted for a toolchain, choose **MinGW** (bundled) or your configured
   **Visual Studio** toolchain.
4. Let CLion finish loading CMake. The first load fetches GLFW and Dear ImGui,
   so it needs internet access and can take a minute.
5. Select the `CursedRL` run configuration in the top toolbar.
6. Click **Run** or press **Shift+F10**.

Dependencies remain inside the local build directory; nothing is installed
system-wide.

## Optional CLion presets

The included `CMakePresets.json` defines `clion-debug` and `clion-release`
Ninja builds. CLion can import these automatically. Its ordinary generated
Debug profile also works, so using the presets is optional.

If CMake loading fails while fetching dependencies, confirm that Git is
available from CLion's terminal and reload the CMake project.

## Controls

- Arrow keys: cardinal movement
- Numpad: eight-direction movement
- Move into an allied Shikigami to swap places with it
- Walk onto a Cursed Pachinko Machine to open its minigame
- Walk onto a Cursed Tool Shop to browse its stock
- Space or numpad 5: wait one turn
- Hold a movement or wait key to repeat it
- Mouse wheel over the dungeon: zoom in or out
- New runs begin at 150% zoom
- At 100% zoom, dungeon tiles are 32×32 pixels. The 45×29 map is larger than
  the default window and is viewed through the player-centered scrolling camera.
- Units use a separate high-visibility 4×4 map atlas with simplified silhouettes
  for players, spirits, Ronin, Cursed Hosts, Centipedes, and Shadow Wolves. The
  separate detailed sprites remain in use for large inspection portraits.
- The dungeon, message log, and sidebar use inset scrolling viewports so map
  content and obsidian-gold scrollbars remain inside their ornamental frames.
- Hold the middle mouse button and drag: pan the dungeon
- Left-click a map tile: select and inspect it
- Right-click the map: clear inspection
- 1: use the selected clan's Lapse ability
- While aiming: use arrows or numpad directions; numpad 5 confirms
- While aiming Limitless: left-click a visible enemy to select it
- While aiming Volcanic Blast: left-click a map tile to aim toward it
- H: use Reversed Cursed Technique: Healing, or Jogo's innate Cursed Healing
- A: toggle Domain Amplification, if learned
- V: toggle Hollow Wicker Basket, if learned
- B: open or close the Binding Vows menu
- C: view the Grade Advancement screen
- Q: activate the equipped Cursed Tool; Q or Space confirms a Kamutoke target
- `?` (Shift + `/`): open the full Game Controls screen
- Walk into an enemy to attack
- R after death, or the **New game** button: restart

Every successful movement, attack, blocked movement, or wait advances one turn.
Enemies inside their awareness radius move toward the player and attack when
adjacent. After the player moves, the camera recenters on the player.

Moving the mouse over the dungeon displays a tile outline. Left-click pins the
selector and opens a separate movable, closable Inspection panel. Visible units report
their name, health, energy, classification, sorcerer status, attack type, and
Knowledge status. Explored terrain, stairs, and Ronin corpses can also be
inspected. Units outside current vision are never revealed by inspection.
Right-clicking the map or closing the panel clears the selection.

When the player dies, the dungeon is replaced by a Game Over screen showing the
clan, turns survived, remaining Cursed Energy, and selected trait. From there,
the player can return to character creation, open the main menu, or quit.

## Dungeon levels

A run contains fifteen procedurally generated dungeon levels. Each level is
created only when first entered, then preserves its explored tiles, surviving
enemies, and Ronin corpses while the player is elsewhere. Walk onto a blue `<`
to ascend or an amber `>` to descend. Shadow Wolves travel between levels with
the player.

Enemy positions and counts are randomized from a scaling minimum:

| Level | Insect Curses | Ronin | Samurai | Centipede Curses | Spider Ghouls | Bosses |
|---|---:|---:|---:|---:|---:|---:|
| 1 | 4–6 | 1–2 | 0 | 0 | 0 | 0 |
| 3 | 8–10 | 3–4 | 0 | 1 | 1 | 1 |
| 6 | 14–16 | 6–7 | 3–4 | 2 | 2 | 1 |
| 9 | 20–22 | 9–10 | 4–5 | 3 | 3 | 1 |
| 12 | 26–28 | 12–13 | 6–7 | 4 | 4 | 1 |
| 15 | 32–34 | 15–16 | 7–8 | 5 | 5 | 4 |

Each deeper level adds two to the minimum Insect Curse count and one to the
minimum Ronin count. Binding Vow Knowledge remains attached to enemies even
when the player changes levels.

Levels 3, 6, 9, and 12 are boss floors. Both stairways are sealed until the
boss is defeated and its dropped **Cursed Key** is collected. A boss is
randomly selected from Gojo, Zenin, Nanami, or Jogo. Defeating any boss raises
a hidden run statistic called **Boss Power Level**, which begins at 1 and is
used when later floors are generated.

The forward exit on each of these four boss floors is represented by a unique
gate sprite with matching locked and unlocked variants. Its name is rolled once
when the floor is first generated using the `Gate of <name>` convention, from a
pool including Gate of Resentment, Gate of Obsession, Gate of Glory, Gate of
Despair, Gate of Power, Gate of Grace, Gate of Corruption, Gate of Love, and
the other configured virtues and afflictions. The locked sprite displays pink
cursed seals and chains; collecting the boss's key changes it to its matching
open cyan-lit variant. The selected name appears in messages, inspection, and
the Dungeon sidebar.

Boss statistics are generated from that hidden level:

- HP: `40 + 20 Ã— Boss Power Level`
- Maximum Cursed Energy: `100 + 25 Ã— Boss Power Level`
- Base melee damage: `5 + 5 Ã— Boss Power Level`
- Melee attacks per turn: equal to Boss Power Level
- Clan-technique damage: `5 + 5 Ã— Boss Power Level`
- Power 1: Grade 3
- Power 2: Grade 2
- Power 3: Grade 1
- Power 4 or higher: Special Grade

Every boss uses Cursed Energy Reinforcement offensively and defensively at the
standard rate of 5 CE per point. On each melee strike it adds up to Boss Power
Level damage, and against each incoming hit it blocks up to Boss Power Level
damage. Human equipment bonuses are applied in addition to the generated base
melee damage.

Bosses generated at Power 3 or higher independently roll the ability to use
Reversed Cursed Technique: Healing, Simple Domain, Domain Amplification, and
Hollow Wicker Basket. Only Power 4 or higher bosses can expand a Domain. Gojo
bosses always possess Neutral Limitless and gain Cursed Technique Reversal: Red
at Power 4.

Human bosses are named `Grade + Clan + Sorcerer`, such as `Grade 2 Zenin
Sorcerer`. Jogo bosses are curses and use `Grade + Curse + Clan`, such as
`Special Grade Curse Jogo`.

Level 15 contains all four possible bosses simultaneously. Each drops one
Cursed Key. Recover all four and walk into the **Gate of Heaven** to complete
the run and reach the victory screen.

### Grade Advancement

There are no Cursed Idol tiles in the dungeon. Press `C` at any time during
gameplay to view the **Grade Advancement** screen. The screen no longer opens
automatically when a rank threshold is reached. Before the next grade has been
earned, its improvements remain visible but disabled. Once a new grade is
earned, run points can be spent on permanent improvements for the current run.
Leaving an available advancement with unspent points opens a confirmation
warning: retained points cannot be used for Grade Advancement upgrades again
until the next grade level is reached.

- 5 points: +10 maximum and current HP
- 5 points: +2 base attack
- 5 points: +10 maximum and current Cursed Energy
- 5 points: +5% Cursed Energy Efficiency
- 5 points: +2 Cursed Technique Power damage
- 5 points: +1 Close Combat Ability, granting one additional melee attack
- 5 points: +1 Cursed Energy Generation per turn

Each option has its own escalating price. After purchasing an upgrade, the next
purchase of that same option costs 1 additional point. Other upgrade prices are
unaffected, and all prices reset to 5 when a new run begins.

The Cursed Technique Power bonus is added to damaging clan techniques before
Technique Output and Hand Signs multipliers. For Ratio, it is added to the
marked strike before the critical multiplier is applied. Increasing this stat
does not increase a technique's Cursed Energy cost.

Starting at Grade 2, the screen teaches **Reversed Cursed Technique: Healing**
for 10 points. After learning it, choose a base energy spend in 10-point
increments in the sidebar and press `H`. Each 10 base energy heals 5 HP. Cursed
Energy Efficiency reduces the amount actually paid, and healing consumes one
turn. Escape closes Grade Advancement; it becomes available again when the
player reaches another rank.

Jogo clan characters are classified as Cursed Spirits and cannot learn Reversed
Cursed Technique. They automatically know **Cursed Healing**, which uses the
same sidebar controls, energy costs, efficiency scaling, healing amounts, and
turn cost.

At Grade 1, any player clan can spend 50 points to learn **Domain Expansion**.
Press `D` to preview the five-tile-radius area in the clan's color, then press
`D`, Space, or numpad 5 to confirm; Escape cancels. Press `D` while active to
release it. Activation costs 40 base Cursed
Energy; each later active turn costs 20 base energy. Every active turn deals
the player's customized Cursed Technique damage to enemies inside, and those
enemies cannot move or attack. The player also cannot move while maintaining
the domain. Every affected tile is highlighted with the selected clan's color.
The domain collapses if upkeep cannot be paid. After this ability is learned,
use Right Arrow or numpad 6 for rightward movement because `D` controls it.

When the player's Domain Expansion overlaps a hostile boss domain, a **Domain
Clash** begins. Both domains continue paying normal upkeep, but their sure-hit
technique damage is negated and accumulated. The first domain that can no
longer be maintained collapses inside the surviving domain. The loser then
takes the winner's total accumulated technique damage, and the winner restores
half of their maximum Cursed Energy. The sidebar displays `DOMAIN CLASH` while
this interaction is active.

Each clan has a uniquely named, clan-colored domain:

- Gojo: **Infinite Void**
- Zenin: **Chimera Shadow Garden**
- Jogo: **Coffin of the Iron Mountain**
- Nanami: **Unpaid Overtime**

Nanami's Domain Expansion replaces the normal domain damage tick. Each active
turn gives every enemy inside one Ratio stack and immediately makes one normal
player melee attack against each enemy. The newly applied stack guarantees the
attack's Ratio critical hit and is consumed by that strike.

Zenin's Domain Expansion also replaces the normal damage tick. It creates one
temporary **Domain Gyokuken** on an open adjacent tile beside each enemy caught
inside. If every adjacent tile is blocked, that summon waits until space is
available. Domain Gyokuken do
not count toward the normal summon limit, require no Cursed Energy upkeep, and
take ally turns normally. They disappear when the domain is released or
collapses.

Whenever Domain Expansion is released or collapses, the player gains
**Technique Burnout** for 10 full turns. During burnout, clan techniques,
Reversal: Red, Neutral Limitless, Domain Expansion, Shikigami techniques,
Simple Domain, and cursed-technique healing are unavailable. Active Neutral
Limitless and Shikigami end when burnout begins. Cursed Energy Reinforcement
continues to function normally.

## Points

### Cursed Pachinko Machine

One **Cursed Pachinko Machine** appears every three levels, on levels 3, 6, 9,
12, and 15. Walking onto its
gold-and-violet machine tile opens a playable pachinko panel. Available run
points purchase balls at a rate of 1 point per ball. Balls fall through a
physics-driven peg field and land in thirteen payout pockets. Each machine
rolls its layout once when its dungeon level is first generated and retains it:

- 70%: `0, 0, 1, 0, 2, 0, 5, 0, 2, 0, 1, 0, 0`
- 20%: `1, 0, 1, 0, 2, 0, 3, 0, 2, 0, 1, 0, 1`
- 10%: `5, 0, 0, 5, 0, 0, 10, 0, 0, 5, 0, 0, 5`

Launch 1, 5, or every available ball at once; all launched balls remain active
together.

Pocket payouts award additional balls, not points. To cash out, exchange balls
for a Cursed Card (20), Cursed Doll (50), or Cursed Finger (100). Each object
returns spendable points equal to its ball price but does not increase total
points earned or player rank. Balls and collected object counts remain for the
current run, and operating the panel does not advance dungeon turns.

The run score increases whenever the player, a summoned ally, or reflected
player damage defeats an enemy:

- Grade 4 Cursed Spirit: 1 point
- Grade 3 Cursed Spirit: 3 points
- Human / Non-sorcerer: 2 points
- Cursed Host: 5 points

Point tracking is split into four values:

- **Current points total:** spendable points currently held.
- **Earned Points:** enemy-kill points earned toward the next Grade
  Advancement; reset to zero when that advancement is completed.
- **Total Earned Points:** all points earned from defeating enemies during the
  run. Purchases, Pachinko cash-outs, and debug grants do not change it.
- **Spent Points:** the cumulative number of points spent on advancements,
  shops, and Pachinko balls.

### Player rank

Rank advancement is based on the resettable **Earned Points** counter:

- Grade 4: starting rank
- Grade 3: earn 15 points while Grade 4
- Grade 2: earn 50 points while Grade 3
- Grade 1: earn 100 points while Grade 2
- Special Grade: earn 200 points while Grade 1

After completing an advancement, Earned Points reset to zero. Special Grade is
repeatable: every additional 200 Earned Points unlocks another Grade
Advancement screen while the player's rank remains Special Grade.

Ranks are color-coded in the sidebar, player inspection, and Game Over screen.
Special Grade alternates between gold and magenta whenever the player changes
dungeon levels.

### Debug menu

Press the backtick key (`` ` ``) during play to open the Debug Menu. Its
buttons add 10, 100, or 1000 points to the player's current and grade-cycle
Earned Points, allowing advancement testing without changing Total Earned
Points. Debug grants do not consume a turn and trigger rank advancement
normally. **Reveal Current Map** permanently marks every tile on the current
dungeon level as explored without revealing units outside the player's normal
field of view. Press backtick or Escape to close the menu.

### Boss equipment

Human sorcerer bosses carry and equip a Black Training Uniform, Steel Katana,
and Wrapped Cursed Tanto. The uniform reduces direct damage by 25%, the Katana
multiplies base Attack by 1.5, and the Tanto makes melee damage cursed. All
three items appear in the inspection panel and drop when the boss dies. Human
sorcerer bosses cannot become Cursed Hosts.

### Cursed Spirits

**Insect Curses** are classified as **Grade 4**. Each has 5 HP and 40 Cursed
Energy. Ordinary weapon damage cannot harm one. It can be damaged only by a
Cursed Technique, Gyokuken attacks, the
bonus damage supplied by attack reinforcement, or Serrated Cursed Energy
reflection. When valid damage reaches it, the curse automatically spends 5
Cursed Energy per point to block as much of that damage as possible. Once its
energy is exhausted, valid damage reaches its health without mitigation.
Exorcising it restores one quarter of its maximum Cursed Energy pool—10 points—without
exceeding the player's maximum.

Each Insect Curse randomly receives one attack type when the dungeon is
created:

- Purple `B` — **Bite:** one attack dealing 7 damage
- Purple `K` — **Claws:** two separate attacks dealing 3 damage each
- Purple `S` — **Stinger:** deals 3 damage and Poisons the player for 3 turns

Poison deals 3 direct damage after each of the player's next three actions.
Defense reinforcement does not block poison. Claws resolves as two distinct
hits, so defense reinforcement and Serrated reflection apply separately to
each hit.

An Insect Curse that sees an uncursed Ronin will pursue it. On reaching an
adjacent tile, the spirit can spend its turn embedding its curse into the
Ronin. The spirit remains in play, while the affected Ronin becomes a
purple-outlined **Cursed Host** and inherits that spirit's Bite, Claws, or
Stinger technique.

Starting on dungeon level 2, levels contain increasingly many **Grade 3
Centipede Curses**. The count begins at one and gains another Centipede every
three levels, reaching five on level 15. A Centipede Curse has 15 HP and 80
Cursed Energy. On each attack turn it uses **Bite** for 7 damage and then
**Stinger** for 3 damage, with Stinger applying the normal three-turn Poison.
The second technique is skipped if Bite defeats its target. It uses the same
Cursed Energy reinforcement rules as other spirits, awards 3 points when
exorcised, and restores one quarter of its maximum energy pool to the player.

A Centipede Curse can embed itself into a Ronin. The resulting **Centipede
Cursed Host** inherits both Bite and Stinger and spends 20 Cursed Energy to use
both in the same turn, falling back to Katana Slash when it cannot afford the
pair. It uses dedicated map and inspection sprites distinct from an ordinary
Cursed Host.

Starting on dungeon level 2, **Spider Ghouls** use the same spawn progression
as Centipede Curses: one on levels 2–3, two on levels 4–6, three on levels
7–9, four on levels 10–12, and five on levels 13–15. They are classified as
**Grade 3 Cursed Spirits**. A Spider Ghoul has
35 HP, 80 maximum Cursed Energy, 1 CE generation per turn, 15 melee damage,
an awareness range of 10, and awards 3 points when exorcised. It can use
Cursed Energy Reinforcement and cannot use a Domain Expansion.

Its **Gossamer Snare** costs 20 CE, has range 5, deals 5 cursed damage in a
2-tile radius around the player, and inflicts **Restrained** for 3 turns.
Restrained prevents movement but still permits attacks and abilities. The
Spider Ghoul uses its ranged technique while the player is in range and not
Restrained, then closes for melee while the web holds them. Below half health,
it spends CE to restore 3 HP per point. It prefers the player but can embed its
curse into a Ronin, creating a dedicated **Spider Cursed Host** with Gossamer
Snare, reinforcement, healing, and separate map and inspection sprites.

### Ronin

Two silver `R` enemies spawn alongside the Insect Curses. A Ronin is classified
as **Human / Non-sorcerer** and has 20 HP and 100 Cursed Energy. An ordinary
Ronin cannot spend that energy and its **Katana Slash** deals exactly 5 damage.

Starting on level 4, **Samurai** appear as elite Ronin at approximately half
that level's Ronin spawn range, rounded into a two-value range. Level 6 creates
3–4 Samurai, level 9 creates 4–5, level 12 creates 6–7, and level 15 creates
7–8.
A Samurai is a Human / Non-sorcerer with 50 HP and 100 Cursed Energy. Its
**Iaijutsu Strike** deals 15 damage. Unlike a Ronin, a Samurai rejects Grade 4
possession: only Grade 3 or stronger Cursed Spirits can turn it into a Cursed
Host. The resulting Samurai Cursed Host retains its 15-damage Iaijutsu Strike,
gains the possessing curse's technique and reinforcement, and uses dedicated
gameplay and inspection sprites.

Once cursed, a Ronin uses its embedded technique for 10 Cursed Energy whenever
possible; if it cannot afford the technique, it falls back to Katana Slash. A
Cursed Host also reinforces each attack for +1 damage and reinforces its
defense to block 1 damage, with each reinforcement costing 5 Cursed Energy.
When below 50% health, it spends its turn healing at a rate of 3 HP per 1
Cursed Energy, spending only enough energy to fill its missing health when
possible.
Damage from its embedded technique counts as Cursed Technique damage and can
harm other cursed spirits. It remains Human / Non-sorcerer for scoring and
corpse rules.

Unlike a cursed spirit, a Ronin can be harmed by ordinary melee attacks and
grants no Cursed Energy when defeated. A defeated Ronin leaves a persistent,
non-blocking `%` corpse on its death tile.

## Binding vows

Press `B` during play to open the Binding Vows menu. **Divulge Cursed
Technique** gives every currently visible enemy persistent Knowledge, causing
each of them to deal +1 damage. In exchange, the player gains +25% Cursed
Energy Efficiency until every enemy marked with Knowledge is defeated. Marked
enemies have a gold outline. The vow cannot be accepted again while active.

**Hand Signs** is a toggleable Binding Vow in the same menu. It uses hand signs
to improve Cursed Technique power by 1.25x but prevents the player from using
the Weapon equipment slot. Activating it immediately returns an equipped
weapon to the inventory, and weapons cannot be equipped until the vow is
turned off. It increases Limitless damage, Gyokuken damage, Disaster Flames
damage, and Ratio's empowered strike after Technique Output is applied.

## Inventory and equipment

Sorcerer characters choose a starting item and can carry, collect, and equip
items. Cursed-spirit characters such as Jogo have no inventory or equipment,
skip starting-item selection, cannot pick up drops, and do not receive the
inventory control or equipment panel in the sidebar.

Press `I` during play to open the Inventory screen. Opening the menu
pauses gameplay input, and equipping or removing an item does not consume a
turn. Every carried item displays its equipment type, gameplay stats, and
description, with an **Equip** or **Unequip** button. The player has two
equipment slots:

The inventory displays dedicated illustrated icons for the Black Training
Uniform, Steel Katana, and Wrapped Cursed Tanto in both the equipped summary
and carried-item list.

- **Clothing** — the Black Training Uniform reduces incoming direct-hit damage
  by 25%, rounded down. This reduction is applied before defensive Cursed Energy
  Reinforcement.
- **Weapon** — the Steel Katana multiplies the player's base Attack by 1.5,
  rounded upward. The Wrapped Cursed Tanto is also a weapon and causes melee
  attacks to count as cursed damage, allowing them to harm cursed spirits.

Only one item can occupy each slot. Equipping another item of the same type
automatically replaces the currently equipped item.

Each defeated Ronin independently has a 50% chance to drop a Steel Katana and
a 50% chance to drop a Black Training Uniform. If that Ronin was a Cursed Host,
it also has a 50% chance to drop a Wrapped Cursed Tanto. Dropped clothing `[`,
and weapons `)` appear on the map and are picked up by
stepping onto their tile.

Equipping the Wrapped Cursed Tanto converts the player's entire mundane melee
strike into cursed damage. Spending at least one point of attack reinforcement
does the same, rather than making only the reinforcement bonus capable of
harming a curse.

## Usable Cursed Tools

Every dungeon floor contains a Cursed Tool Shop. Walking onto it opens a shop
that sells tools for available points. A Cursed Bandage costs 1 point and
stacks with other bandages. Owned tools appear in the normal Inventory and can
be equipped in the **Cursed Tool Inventory** slot. Pressing `Q` activates the
equipped Bandage, restoring 10 Health and consuming one from the stack.

An Imperfect Kamutoke costs 5 points and begins with three uses. Equip it, press
`Q`, choose a visible enemy with the mouse, arrow keys, numpad, or Tab, then
press `Q`, Space, or numpad 5 to confirm. It spends 10 Cursed Energy and deals
20 cursed lightning damage. The tool breaks after its third use. Empty tools
are removed from Inventory automatically, and tool status is hidden when the
player owns no tools. Player Cursed Spirits cannot carry inventory items.

The shop also sells two 5-point accessories. Only one can occupy the new
**Accessory** equipment slot. The **Cursed Blindfold** adds 10% Cursed Energy
Efficiency while equipped. The **Cursed Pin** adds 20 maximum Cursed Energy;
unequipping it clamps current energy back to the normal maximum if necessary.

## Project structure

```text
CursedRL/
├── CMakeLists.txt       Dependencies and cross-toolchain build configuration
├── CMakePresets.json    Optional CLion Debug and Release presets
├── README.md            Setup, controls, and design notes
└── src/
    └── main.cpp         Game state, turns, FOV, rendering, and application loop
```

This intentionally keeps the starter in one well-commented source file so the
core loop is easy to study. Natural next extractions are `Game`, `Dungeon`,
`Actor`, and `Renderer` modules. Saving, procedural room placement, inventory,
and pathfinding can then be added without changing the platform layer.
