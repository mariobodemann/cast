#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cmath>
#include <termios.h>
#include <unistd.h>
using namespace std;

struct Settings;

static void title(string, const Settings&);
static void render(const Settings& settings);
static void info(string);
static void error(string);
static void error(string, string);

#define PI 3.14159265f

#define FILL_CHARACTERS 0
#define FILL_COLOURED_CHARACTERS 1
#define FILL_BACKGROUND_AND_CHARACTERS 2
#define FILL_BACKGROUND 3
#define FILL_LENGTH FILL_BACKGROUND +1

union V {
	struct {
		float x;
		float y;
	};

	struct {
		float w;
		float h;
	};

	struct {
		float width;
		float height;
	};

	struct {
		float i;
		float j;
	};
};

union M {
	struct {
		float a11, a12;
		float a21, a22;
	};
	struct {
		float a,b,c,d;
	};
	struct {
		float m[4];
	};
};

M rot(float angle) {
	float c = cos(angle);
	float s = sin(angle);
	M m = { c, -s, s, c };
	return m;
}

V times (M m, V v) {
	V r = {m.a * v.x + m.b * v.y, m.c* v.x + m.d * v.y};
	return r;
}


V vplus(V v1, V v2) {
	V v = {v1.x + v2.x, v1.y + v2.y};
	return v;
}

V vminus(V v1, V v2) {
	V v = {v1.x - v2.x, v1.y - v2.y};
	return v;
}

V times(V v, float s) {
	V r = {v.x * s, v.y * s};
	return r;
}

V normalize(V v) {
	float l = (float)sqrt(v.x * v.x + v.y * v.y);
	return times(v, 1.0f/l);
}

struct Player {
	V pos;
	V dir = {0.f, -1.f};
};

struct World {
	V size;
	char* cells = NULL;
};

struct Screen {
	V size = {75,25};
};

struct Camera {
	float distance;
	float width;
};

struct Settings {
	Player player;
	World world;
	Screen screen;
	Camera camera = {10.0f, 10.0f};

	bool minimap = false;
	int mode = 0;

	Settings(int argc, char** argv){
		for(int i = 1; i < argc; ++i) {
			bool hasArg = i + 1 < argc;

			string arg(argv[i]);
			if( arg.compare("w") == 0 && hasArg) {
				if (0 < (screen.size.w = atoi(argv[++i]))) {
					cout << "screen width set to '" << screen.size.w << "'" << endl;
				} else {
					error("Parameter not in range: w");
				}
			} else if( arg.compare("h") == 0 && hasArg) {
				if (0 < (screen.size.h = atoi(argv[++i]))) {
					cout << "screen height set to '" << screen.size.h << "'" << endl;
				} else {
					error("Parameter not in range: h");
				}
			} else if( arg.compare("D") == 0 && hasArg) {
				camera.distance = atof(argv[++i]);
				cout << "camera distance set to '" << camera.distance << "'" << endl;
			} else if( arg.compare("W") == 0 && hasArg) {
				camera.width = atof(argv[++i]);
				cout << "camera width set to '" << camera.width << "'" << endl;
			} else if( arg.compare("l") == 0 && hasArg) {
				loadWorld(string(argv[++i]));
			} else {
				error("Could not parse argument ", arg);
			}
		}

		if (world.cells == NULL) {
			loadWorld("level");
		}

		if (!verify()) {
			error("Could not create world, did you add all parameters?");
		}
	}

	bool verify() {
		if (screen.size.width <= 0) {
			info("screen width invalid");
			return false;
		}
		if (screen.size.height <= 0) {
			info("screen height invalid");
			return false;
		}

		if (world.size.width <= 0) {
			info("world width to small");
			return false;
		}
		if (world.size.height <= 0) {
			info("world height to small");
			return false;
		}
		if (world.cells == NULL) {
			info("cells not set");
			return false;
		}
		return true;
	}

	virtual ~Settings() {
		if (world.cells) {
			free(world.cells);
		}
	}

private:
	void loadWorld(string levelFileName) {
		fstream fs(levelFileName, fstream::in);

		if (!fs.is_open()) {
			error("could not read file", levelFileName);
			return;
		}
		
		string temp;
		if(!getline(fs, temp)) {
			error("could not find width");
		}
		if (0 >= (world.size.width = atoi(temp.data()))) {
			error("width was to small: ", temp);
		}
		
		if (!getline(fs, temp)) {
			error("height was not found");
		}	
		if (0 >= (world.size.height = atoi(temp.data()))) {
			error("width was to small: ", temp);
		}

		int index = 0;
		world.cells = (char*)calloc(world.size.width * world.size.height, sizeof(char));
		for (int y = 0; y < world.size.height; ++y) {
			if (!getline(fs,temp)) {
			       error("line could not be parsed: ",  to_string(y));
			}

			for (int x = 0; x < world.size.width; ++x) {
				world.cells[index++] = temp[x];
			}
		}
		
		if (!getline(fs, temp)) {
			error("player x was not found");
		}	
		player.pos.x = atoi(temp.data());

		if (!getline(fs, temp)) {
			error("player y was not found");
		}	
		player.pos.y = atoi(temp.data());

		fs.close();
	}
};

struct termios old_term;
void setup_terminal() {
        tcgetattr(STDIN_FILENO, &old_term);
	
	struct termios t;
        tcgetattr(STDIN_FILENO, &t);
        t.c_lflag &= ~ICANON;
        t.c_lflag &= ~ECHO;
        
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void cleanup_terminal() {
        old_term.c_lflag |= ICANON;
        old_term.c_lflag |= ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
}

float dtor(float angle) {
	return ((float)(PI/180.0f) * angle);
}

int main(int argc, char** argv) {
	atexit(cleanup_terminal);
	setup_terminal();
		
	srand(time(NULL));
	
	Settings settings(argc, argv);
	title("ASCII CAST", settings);

	cout << endl;
	cout << endl;

	M rot_plus = rot(dtor(5));
	M rot_minus = rot(dtor(-5));

	int i = 0;
	render(settings);       
	while(i != 27) {
		
		i = cin.get();
		if ( i == 'w' ) {
			settings.player.pos = vplus(settings.player.pos, settings.player.dir);
		} else if ( i == 's') {
			settings.player.pos = vminus(settings.player.pos, settings.player.dir);
		} else if ( i == 'a') {
			settings.player.pos = vplus(settings.player.pos, times(rot(dtor(90)), settings.player.dir));
		} else if ( i == 'd') {
			settings.player.pos = vplus(settings.player.pos, times(rot(dtor(-90)), settings.player.dir));
		}
		
		if ( i == 'e') {
			settings.player.dir = times(rot_plus, settings.player.dir);
		} else if ( i == 'q') {
			settings.player.dir = times(rot_minus, settings.player.dir);
		}

		if (i == 'm'){
			settings.minimap = !settings.minimap;
		}

		if (i == 'v'){
			settings.mode ++;
			if (settings.mode == FILL_LENGTH) {
				settings.mode = 0;
			}
		}

		cout << "\033[" << settings.screen.size.height << "A";
		render(settings);
	}
}

bool drawScreenFrame(int x, int y, int w, int h) {
	if (x == 0) {
		if ( y == 0) {
			cout << "┌";
			return true;
		} else if (y == h - 1) {
			cout << "└";
			return true;
		} else {
			cout << "│";
			return true;
		}
	} else if (x == w -1) {
		if ( y == 0) {
			cout << "┐";
			return true;
		} else if (y == h -1) {
			cout << "┘";
			return true;
		} else {
			cout << "│";
			return true;
		}
	} else if (y == 0 || y == h - 1) {
		cout << "─";
		return true;
	}

	return false;
}

void drawCell(char cell, int mode){
	if (cell == ' '){
		cout << cell;
	} else {
		switch(mode){
			default:
			case FILL_CHARACTERS:
				cout << cell;
				break;
			case FILL_COLOURED_CHARACTERS:
				cout << "\033[" << 31 + cell % 7 << "m" << cell << "\033[m";
				break;
			case FILL_BACKGROUND_AND_CHARACTERS:
				cout << "\033[" << 41 + cell % 7 << "m" << cell << "\033[m";
				break;
			case FILL_BACKGROUND:
				cout << "\033[" << 41 + cell % 7 << "m \033[m";
				break;
		}
	}
}

bool drawMiniMap(int x, int y, const Settings& settings) {
	World world = settings.world;
	Player player = settings.player;

	V arrow_position = {player.pos.x + player.dir.x, player.pos.y + player.dir.y};

	string arrow; 
	int ax = round(arrow_position.x);
	int ay = round(arrow_position.y);
	int px = int(player.pos.x);
	int py = int(player.pos.y);
	if (ax == px) {
		if (ay < py) {
			arrow = "^";
		} else {
			arrow = "V";
		}
	} else if (ax < px) { 
		if (ay == py) {
			arrow = "<";
		} else if (ay < py) {
			arrow = "\\";
		} else if (ay > py) {
			arrow = "/";
		}
	} else if (ax > px) { 
		if (ay == py) {
			arrow = ">";
		} else if (ay < py) {
			arrow = "/";
		} else if (ay > py) {
			arrow = "\\";
		}
	}

	if (x >= 0 && y >= 0 && x < world.size.width && y < world.size.height) {
		if (px == x && py == y) {
			cout << "\033[32m@\033[m";
		} else if (ax == x && ay == y) {
			cout << "\033[31m" << arrow << "\033[m";	
		} else {
			drawCell(world.cells[y*((int)world.size.width) + x], settings.mode);
		}
		return true;
	}
	return false;
}

void rayCast(int x, int y, const Settings& settings) {
	Screen screen = settings.screen;
	Camera camera = settings.camera;
	World world = settings.world;
	Player player = settings.player;

	V playerToPlane = times(player.dir, camera.distance);
	V planeDir = times(rot(dtor(90.0f)), player.dir);
	V planeStart = vplus(player.pos, vplus(playerToPlane, times(planeDir, camera.width / 2.0f)));
	V rayOnPlane = vplus(planeStart, times(times(rot(dtor(180.0f)), planeDir), camera.width * (float)x / screen.size.width));
	V rayDir = vminus(rayOnPlane, player.pos);

	bool hit = false;
	for (float factor = 0.1f; factor < 100.f && !hit; factor += 0.1f) {
		V check = vplus(player.pos, times(rayDir, factor));
		if ( int(check.x) >= 0 && int(check.x) < world.size.width 
		  && int(check.y) >= 0 && int(check.y) < world.size.height) {
			char cell = world.cells[int(check.y) * int(world.size.w) + int(check.x)];

			if (cell != ' ') {
				float h = 1.0f / factor;

				if (abs(screen.size.height / 2.0f - y) < h) {
					drawCell(cell, settings.mode);
				} else {
					cout << " ";
				}

				hit = true;
			}
		} else {
			factor += 100.0f;
		}
	}

	if (!hit) {
		cout << ".";
	}
}

void render(const Settings& settings) {
	for (int y = 0; y < settings.screen.size.height; ++y) {
		for ( int x = 0; x < settings.screen.size.width; ++x) {
			if ( !drawScreenFrame( x, y, settings.screen.size.width, settings.screen.size.height)
				&& (!settings.minimap || !drawMiniMap( x - 1, y - 1, settings))
			) {
				rayCast( x, y, settings);
			}
		}
		cout << endl;
	}
}

string maze(int length) {
	string result;
	for( int i = 0; i < length; ++i) {
		result = result + ((rand() % 2) == 0 ? "╱" : "╲");
	}
	return result;
}

void title(string message, const Settings& settings){
	int w = settings.screen.size.width;
	int w2 = w/2;
	
	cout << maze(w) << endl;
	cout << maze(w) << endl;
	cout << maze(w2 - message.length() / 2 - 2) << "  " << string(10, ' ') << "  " << maze(w2 - message.length() / 2 - 2) << endl;
	cout << maze(w2 - message.length() / 2 - 2) << "  " << message << "  " << maze(w2 - message.length() / 2 - 2) << endl;
	cout << maze(w2 - message.length() / 2 - 2) << "  " << string(10, ' ') << "  " << maze(w2 - message.length() / 2 - 2) << endl;
	cout << maze(w) << endl;
	cout << maze(w) << endl;
}

void info(string message) {
	cout << "i: " << message << endl;
}

void error(string message) {
	error(message, "");
}

void error(string message, string additional) {
	cerr << "Error occured: " << message << additional << endl;
	exit(-1);
}

