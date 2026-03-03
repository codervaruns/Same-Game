# Same Game - Java Version Build Instructions

## Prerequisites

### Required Software
1. **Java Development Kit (JDK) 11 or higher**
   - Download from: https://adoptium.net/ or https://www.oracle.com/java/technologies/downloads/
   - Verify installation: `java -version` and `javac -version`

2. **Maven 3.6 or higher** (for building with Maven)
   - Download from: https://maven.apache.org/download.cgi
   - Verify installation: `mvn -version`

## Project Structure

```
Same-Game/
├── src/
│   └── main/
│       └── java/
│           └── com/
│               └── samegame/
│                   ├── Node.java           # Node structure for graph
│                   ├── SameGame.java       # Game logic class
│                   ├── SameGameGUI.java    # Swing GUI implementation
│                   └── TestGraph.java      # Test suite
├── pom.xml                                 # Maven build configuration
└── BUILD_JAVA.md                          # This file
```

## Building the Project

### Option 1: Using Maven (Recommended)

#### Compile the project:
```bash
mvn clean compile
```

#### Create executable JAR:
```bash
mvn clean package
```

This will create two JAR files in the `target/` directory:
- `same-game-1.0.0.jar` - Regular JAR
- `same-game-1.0.0-shaded.jar` - Fat JAR with all dependencies (not needed for this project)

#### Run the game:
```bash
java -jar target/same-game-1.0.0.jar
```

Or:
```bash
mvn exec:java -Dexec.mainClass="com.samegame.SameGameGUI"
```

### Option 2: Using javac (Manual Compilation)

#### Compile all Java files:
```bash
cd src/main/java
javac com/samegame/*.java
```

#### Run the GUI:
```bash
java com.samegame.SameGameGUI
```

#### Run the tests:
```bash
java -ea com.samegame.TestGraph
```

Note: The `-ea` flag enables assertions for the test suite.

### Option 3: Using an IDE

#### IntelliJ IDEA:
1. Open the project folder in IntelliJ IDEA
2. The IDE will automatically detect the Maven project
3. Right-click on `SameGameGUI.java` and select "Run 'SameGameGUI.main()'"

#### Eclipse:
1. Import as "Existing Maven Project"
2. Right-click on `SameGameGUI.java` and select "Run As > Java Application"

#### VS Code:
1. Install "Extension Pack for Java"
2. Open the project folder
3. Press F5 or click "Run" on `SameGameGUI.java`

## Running the Application

### Run the GUI Game:
```bash
# Using Maven
mvn exec:java -Dexec.mainClass="com.samegame.SameGameGUI"

# Or run the JAR
java -jar target/same-game-1.0.0.jar

# Or directly with java
cd src/main/java
java com.samegame.SameGameGUI
```

### Run the Test Suite:
```bash
# Using Maven
mvn exec:java -Dexec.mainClass="com.samegame.TestGraph"

# Or directly with java (with assertions enabled)
cd src/main/java
java -ea com.samegame.TestGraph
```

## Controls

- **Mouse Hover**: Highlight clusters of same-colored tiles
- **Left Click**: Remove a cluster (minimum 2 tiles of same color)
- **R Key**: Restart game with a new random grid
- **ESC Key**: Quit the game

## Game Rules

1. The game starts with a 6x8 grid of colored tiles (5 colors: Green, White, Red, Blue, Yellow)
2. Player and computer take turns
3. Click on a cluster of 2 or more adjacent tiles of the same color to remove them
4. Score is calculated as: `(cluster_size - 2)²`
5. After removing tiles:
   - Remaining tiles fall down (vertical gravity)
   - Empty columns shift left (horizontal gravity)
6. Game ends when no more valid moves are available
7. Winner is determined by highest score

## Features

### Core Game Logic (SameGame.java)
- Graph-based tile representation
- BFS cluster detection algorithm
- Gravity system (vertical and horizontal)
- Turn-based gameplay (User vs Computer)
- Greedy AI for computer moves

### GUI (SameGameGUI.java)
- Java Swing-based graphical interface
- Mouse hover highlights for clusters
- Visual tile colors and borders
- Score display for both players
- Turn indicator
- Game over detection and restart

### Testing (TestGraph.java)
- Comprehensive test suite
- Tests initialization, cluster detection, gravity, and game logic
- Validates User vs Computer gameplay

## Differences from C++ Version

### Technology Changes:
- **C++ → Java**: Language conversion
- **SDL2 → Java Swing**: GUI framework change
- **Manual memory management → Garbage collection**: Memory handling
- **Headers (.h/.cpp) → Single files (.java)**: File structure

### Code Equivalents:
- `vector<T>` → `List<T>` / `ArrayList<T>`
- `unordered_map<K,V>` → `HashMap<K,V>`
- `unordered_set<T>` → `HashSet<T>`
- `queue<T>` → `Queue<T>` / `LinkedList<T>`
- `pair<T1,T2>` → Custom `Pair<F,S>` class
- `tuple<T1,T2,T3,T4>` → Custom `ClusterInfo` class
- Pointers (`*`) → Object references

### GUI Differences:
- SDL2 rendering → Java Swing components
- SDL2 events → Java AWT event listeners
- SDL_Rect → java.awt.Rectangle
- SDL_Color → java.awt.Color
- TTF fonts → Java Font class

## Troubleshooting

### Issue: "java: package does not exist"
**Solution**: Make sure you're in the correct directory or use Maven to handle compilation.

### Issue: Maven not found
**Solution**: Install Maven and add it to your system PATH.

### Issue: Java version mismatch
**Solution**: Ensure JDK 11 or higher is installed and set as JAVA_HOME.

### Issue: Assertions not working in tests
**Solution**: Run with `-ea` flag to enable assertions: `java -ea com.samegame.TestGraph`

### Issue: GUI not displaying correctly
**Solution**: Make sure you have a display environment. On Linux, you may need to set DISPLAY variable.

## Performance Notes

- The Java version uses similar algorithms as the C++ version
- Java Swing provides smooth 60 FPS rendering via Timer
- Garbage collection handles memory automatically
- Performance is comparable for this game's scale (6x8 grid)

## Development

### Adding New Features:
1. Modify the appropriate Java class
2. Recompile: `mvn clean compile`
3. Test your changes
4. Rebuild: `mvn clean package`

### Code Style:
- Follows Java naming conventions (camelCase for methods/variables)
- Classes start with uppercase letters
- Package name: `com.samegame`
- Proper encapsulation with private/public modifiers

## License

This is a direct conversion from the original C++ implementation, maintaining the same game logic and structure.
