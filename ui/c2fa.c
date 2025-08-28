#include <ctype.h>
#include <ncurses.h>
#include <string.h>
#include "../common.h"

#define INPUT_MAX 256

int ui_show_error(const char *err) {
	short int lines,cols;
	clear();
	getmaxyx(stdscr,lines,cols);
	WINDOW *errwin = newwin(5, cols,(lines/2)-4,0);
	box(errwin,0,0);
	mvwprintw(errwin,0,2,"ERROR");
	mvwaddstr(errwin,1,1,err);
	mvwprintw(errwin,4,(cols-28)/2,"Press any key to continue...");
	wrefresh(stdscr);
	wrefresh(errwin);
	getch();
	delwin(errwin);
	return 0;
}

int create_secret(char **secret, cotp_error_t *err) {
    uint8_t random_bytes[20];
    *err = NO_ERROR;
    if (RAND_bytes(random_bytes, 20) != 1)  return 0; 
    *secret = base32_encode(random_bytes, 20, err);
    if (*err != NO_ERROR || *secret == NULL) return 0;
    return 1;
}

void set_title(WINDOW *win, char *title) { mvwaddstr(win, 0, 2, title); }

/** More deepseek boilerplate tab-selector
 * Creates a tab selector for TOTP digits
 * @param win: window to display selector in
 * @param title: title to display above tabs
 * @return: selected interval (6 or 8) or -1 on cancel
 */
int digits_tab_selector(WINDOW *win, const char *title) {
    int width = 50;
    int height = 8;
    int startx, starty;
    int ch;
    int selected = 0; // Start with 30 seconds selected

    const char *intervals[] = {"6 digits", "8 digits"};
    const char *descriptions[] = {
        "Standard TOTP digits (recommended)",
        "Longer digits"
    };

    // Calculate centered position
    startx = (getmaxx(win) - width) / 2;
    starty = (getmaxy(win) - height) / 2;

    // Create selector window
    WINDOW *selector_win = derwin(win, height, width, starty, startx);
    box(selector_win, 0, 0);
    keypad(selector_win, TRUE);

    // Display title
    mvwprintw(selector_win, 1, (width - strlen(title)) / 2, "%s", title);

    // Input loop
    while (1) {
        // Clear previous content
        for (int i = 2; i < height - 2; i++) {
            wmove(selector_win, i, 1);
            wclrtoeol(selector_win);
        }

        // Draw tabs
        int tab_width = width / 2;
        for (int i = 0; i < 2; i++) {
            int x = i * tab_width + (tab_width - strlen(intervals[i])) / 2;

            if (i == selected) {
                wattron(selector_win, A_REVERSE);
            }

            mvwprintw(selector_win, 3, x, "%s", intervals[i]);

            if (i == selected) {
                wattroff(selector_win, A_REVERSE);
            }
        }

        // Draw description for selected interval
        mvwprintw(selector_win, 5, (width - strlen(descriptions[selected])) / 2,
                 "%s", descriptions[selected]);

        // Draw instructions
        mvwprintw(selector_win, height - 2, 2, "←/→: Switch | ENTER: Select | ESC: Cancel");

        wrefresh(selector_win);

        // Get input
        ch = wgetch(selector_win);

        switch (ch) {
            case KEY_LEFT:
                selected = (selected - 1 + 2) % 2;
                break;

            case KEY_RIGHT:
                selected = (selected + 1) % 2;
                break;

            case '\n':
            case KEY_ENTER:
                delwin(selector_win);
                return selected == 0 ? 6 : 8;

            case 27: // ESC key
                delwin(selector_win);
                return -1;

            default:
                // Handle number keys 1-2 for direct selection
                if (ch == '1') {
                    delwin(selector_win);
                    return 6 ;
                } else if (ch == '2') {
                    delwin(selector_win);
		    return 8;
                }
                break;
        }
    }
}
/** More deepseek boilerplate tab-selector
 * Creates a tab selector for TOTP time intervals
 * @param win: window to display selector in
 * @param title: title to display above tabs
 * @return: selected interval (30 or 60) or -1 on cancel
 */
int step_tab_selector(WINDOW *win, const char *title) {
    int width = 50;
    int height = 8;
    int startx, starty;
    int ch;
    int selected = 0; // Start with 30 seconds selected

    const char *intervals[] = {"30 seconds", "60 seconds"};
    const char *descriptions[] = {
        "Standard TOTP interval (recommended)",
        "Longer interval, less frequent code changes"
    };

    // Calculate centered position
    startx = (getmaxx(win) - width) / 2;
    starty = (getmaxy(win) - height) / 2;

    // Create selector window
    WINDOW *selector_win = derwin(win, height, width, starty, startx);
    box(selector_win, 0, 0);
    keypad(selector_win, TRUE);

    // Display title
    mvwprintw(selector_win, 1, (width - strlen(title)) / 2, "%s", title);

    // Input loop
    while (1) {
        // Clear previous content
        for (int i = 2; i < height - 2; i++) {
            wmove(selector_win, i, 1);
            wclrtoeol(selector_win);
        }

        // Draw tabs
        int tab_width = width / 2;
        for (int i = 0; i < 2; i++) {
            int x = i * tab_width + (tab_width - strlen(intervals[i])) / 2;

            if (i == selected) {
                wattron(selector_win, A_REVERSE);
            }

            mvwprintw(selector_win, 3, x, "%s", intervals[i]);

            if (i == selected) {
                wattroff(selector_win, A_REVERSE);
            }
        }

        // Draw description for selected interval
        mvwprintw(selector_win, 5, (width - strlen(descriptions[selected])) / 2,
                 "%s", descriptions[selected]);

        // Draw instructions
        mvwprintw(selector_win, height - 2, 2, "←/→: Switch | ENTER: Select | ESC: Cancel");

        wrefresh(selector_win);

        // Get input
        ch = wgetch(selector_win);

        switch (ch) {
            case KEY_LEFT:
                selected = (selected - 1 + 2) % 2;
                break;

            case KEY_RIGHT:
                selected = (selected + 1) % 2;
                break;

            case '\n':
            case KEY_ENTER:
                delwin(selector_win);
                return selected == 0 ? 30 : 60;

            case 27: // ESC key
                delwin(selector_win);
                return -1;

            default:
                // Handle number keys 1-2 for direct selection
                if (ch == '1') {
                    delwin(selector_win);
                    return 1;
                } else if (ch == '2') {
                    delwin(selector_win);
                    return 0;
                }
                break;
        }
    }
}

/** Boilerplate extracted from DeepSeek
 * Creates a tab selector for SHA algorithms
 * @param win: window to display selector in
 * @param title: title to display nt in above tabs
 * @return: selected algorithm y_index (0=SHA1, 1=SHA256, 2=SHA512) or -1 on cancel
 */
int sha_tab_selector(WINDOW *win, const char *title) {
    int width = 60;
    int height = 8;
    int startx, starty;
    int ch;
    int selected = 0; // Start with SHA-1 selected

    const char *algorithms[] = {"SHA-1", "SHA-256", "SHA-512"};
    const char *descriptions[] = {
        "Most common algorithm used in TOTP",
        "Another good option",
        "Another good option"
    };

    // Calculate centered position
    startx = (getmaxx(win) - width) / 2;
    starty = (getmaxy(win) - height) / 2;

    // Create selector window
    WINDOW *selector_win = derwin(win, height, width, starty, startx);
    box(selector_win, 0, 0);
    keypad(selector_win, TRUE);

    // Display title
    mvwprintw(selector_win, 1, (width - strlen(title)) / 2, "%s", title);

    // Input loop
    while (1) {
        // Clear previous content
        for (int i = 2; i < height - 2; i++) {
            wmove(selector_win, i, 1);
            wclrtoeol(selector_win);
        }

        // Draw tabs
        int tab_width = width / 3;
        for (int i = 0; i < 3; i++) {
            int x = i * tab_width + (tab_width - strlen(algorithms[i])) / 2;

            if (i == selected) {
                wattron(selector_win, A_REVERSE);
            }

            mvwprintw(selector_win, 3, x, "%s", algorithms[i]);

            if (i == selected) {
                wattroff(selector_win, A_REVERSE);
            }
        }

        // Draw description for selected algorithm
        mvwprintw(selector_win, 5, (width - strlen(descriptions[selected])) / 2,
                 "%s", descriptions[selected]);

        // Draw instructions
        mvwprintw(selector_win, height - 2, 2, "TAB: Switch | ENTER: Select | ESC: Cancel");

        wrefresh(selector_win);

        // Get input
        ch = wgetch(selector_win);

        switch (ch) {
            case '\t': // TAB key
                selected = (selected + 1) % 3;
                break;

            case KEY_LEFT:
                selected = (selected - 1 + 3) % 3;
                break;

            case KEY_RIGHT:
                selected = (selected + 1) % 3;
                break;

            case '\n':
            case KEY_ENTER:
                delwin(selector_win);
                return selected;

            case 27: // ESC key
                delwin(selector_win);
                return -1;

            default:
                // Handle number keys 1-3 for direct selection
                if (ch >= '1' && ch <= '3') {
                    delwin(selector_win);
                    return ch - '1';
                }
                break;
        }
    }
}

/**
 * This was ALL generated by DeepSeek using DeepThink and search, I believe this is boilerplate
 * Creates an input box and gets user input
 * @param win: window to display input box in
 * @param prompt: prompt to display above input field
 * @param buffer: buffer to store input
 * @param buffer_size: size of the buffer
 * @param echo: whether to echo input (0 for hidden, 1 for visible)
 * @return: 0 on success, -1 on cancel (ESC)
 */
int input_box(WINDOW *win, const char *prompt, char **buffer, int buffer_size, int echo) {
    int width = 40;
    int height = 5;
    int startx, starty;
    int ch;
    int pos = 0;
    int cursor_x;

    // Calculate centered position
    startx = (getmaxx(win) - width) / 2;
    starty = (getmaxy(win) - height) / 2;

    // Create input box window
    WINDOW *input_win = derwin(win, height, width, starty, startx);
    box(input_win, 0, 0);

    // Display prompt
    mvwprintw(input_win, 1, (width - strlen(prompt)) / 2, "%s", prompt);

    // Draw input field
    wattron(input_win, A_REVERSE);
    for (int i = 0; i < width - 4; i++) {
        mvwaddch(input_win, 2, 2 + i, ' ');
    }
    wattroff(input_win, A_REVERSE);

    wrefresh(input_win);

    // Initialize buffer
     if (*buffer == NULL) {
        *buffer = malloc(buffer_size);
        if (*buffer == NULL) {
            delwin(input_win);
            return -1;
        }
    }
    (*buffer)[0] = '\0'; 
    pos = 0;

    // Input loop
    while ((ch = wgetch(input_win)) != '\n' && ch != KEY_ENTER) {
        switch (ch) {
            case 27: // ESC key
                delwin(input_win);
                return -1;

            case KEY_BACKSPACE:
            case 127:
            case 8:
                if (pos > 0) {
                    pos--;
                    (*buffer)[pos] = '\0';

                    // Update display
                    wattron(input_win, A_REVERSE);
                    mvwaddch(input_win, 2, 2 + pos, ' ');
                    wattroff(input_win, A_REVERSE);

                    if (echo) {
                        mvwaddch(input_win, 2, 2 + pos, ' ');
                    }
                }
                break;

            case KEY_LEFT:
                if (pos > 0) {
                    pos--;
                }
                break;

            case KEY_RIGHT:
                if (pos < strlen(*buffer)) {
                    pos++;
                }
                break;

            default:
                if (isprint(ch) && pos < buffer_size - 1) {
                    (*buffer)[pos] = ch;
                    (*buffer)[pos + 1] = '\0';

                    if (echo) {
                        mvwaddch(input_win, 2, 2 + pos, ch);
                    } else {
                        mvwaddch(input_win, 2, 2 + pos, '*');
                    }

                    pos++;
                }
                break;
        }

        // Update cursor position
        cursor_x = 2 + pos;
        if (cursor_x >= width - 2) cursor_x = width - 2;
        wmove(input_win, 2, cursor_x);
        wrefresh(input_win);
    }

    delwin(input_win);
    return 1;
}

int setup_ui_new_secret(int index_of_otp, struct OTP *OTPs) {
	cotp_error_t err = NO_ERROR;
	if(!create_secret(&OTPs[index_of_otp].secret, &err)) return ui_show_error("Error creating secret and assigning it to ");
	clear();
	wrefresh(stdscr);
	input_box(stdscr,"Filename", &OTPs[index_of_otp].filename, 31, 1);
	clear();
	refresh();
	OTPs[index_of_otp].algo = sha_tab_selector(stdscr, "Algorithm selection");
	clear();
	refresh();
	OTPs[index_of_otp].period_short = step_tab_selector(stdscr, "Step selection");
	clear();
	refresh();
	OTPs[index_of_otp].digits = digits_tab_selector(stdscr, "Digits selection");
	clear();
	refresh();
	OTPs[index_of_otp].locked = 1;
	return 1;
}


int ui_draw_otp(int y, struct OTP otp, WINDOW *win, cotp_error_t *err, int highlight) {
	// Clear the line first
	wmove(win, y, 1);
	int max_x = getmaxx(win) - 2;
	char str[max_x];
	if (highlight) wattron(win, A_REVERSE);
	for (int i = 1; i <= max_x; i++) {
		mvwaddch(win, y, i, ' ');
	}

	if (otp.filename == NULL) {
		snprintf(str, sizeof(str), "Empty Slot");
		mvwaddstr(win, y, 1, str);
		wattroff(win, A_REVERSE);
		return 1;
	}

	snprintf(str, sizeof(str), "%s: ", otp.filename);
	mvwaddstr(win, y, 1, str);

	if (otp.locked) {
		for (int j = 0; j < otp.digits; j++) {
			int x_pos = max_x - otp.digits + j;
			if (x_pos >= 0 && x_pos < max_x) mvwaddch(win, y, x_pos, '*');
		}
	} else {
		const char *totp_str = get_totp(otp.secret, otp.digits,otp.period_short ? 30 : 60, otp.algo, err);
		if (totp_str != NULL) {
			int x_pos = max_x - strlen(totp_str);
			if (x_pos >= 0) mvwaddstr(win, y, x_pos, totp_str);
		}
	}

	wattroff(win, A_REVERSE);
	return 1;
}

int ui_draw_otps(struct OTP* OTPs, int selected_index, int page, int occupied_otps, WINDOW *win) {
	cotp_error_t err = NO_ERROR;
	int items_per_page = getmaxy(win) - 2;
	int start_index = 0; // Always start from first item for now

	// Clear the window first
	werase(win);
	box(win, 0, 0);
	set_title(win, "OTP List");

	for (int i = 0; i < items_per_page; i++) {
		int index = start_index + i;
		if (index >= occupied_otps) break;

		int y_pos = i + 1;  // +1 to account for border
		int highlight = (selected_index == index) ? 1 : 0;

		if (!ui_draw_otp(y_pos, OTPs[index], win, &err, highlight)) {
			mvwaddstr(win, y_pos, 1, "ui_draw_otp error");
		}
	}
	wrefresh(win);
	return 1;
}
