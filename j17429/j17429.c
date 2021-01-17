#include <GL/glut.h>
#include <GL/glpng.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 11x11の迷路
#define MAPWIDTH 12
#define MAPHEIGHT 11

#define WALL 0
#define ROAD 1

// direction 上=0,右=1,下=2,左=3
#define UP 0
#define DOWN 2
#define LEFT 3
#define RIGHT 1

// ゴールの座標
#define GOALX 288
#define GOALY 288

#define PARTSIZE 32

void Display(void);
void Reshape(int, int);
void Keyboard(unsigned char, int, int);
void SpecialKey(int, int, int);
int isMovable(int, int, int);
void characterMove(int *, int *, int);
void digUp(int *, int *, int *, int[], int[], int[][MAPWIDTH]);
void digDown(int *, int *, int *, int[], int[], int[][MAPWIDTH]);
void digLeft(int *, int *, int *, int[], int[], int[][MAPWIDTH]);
void digRight(int *, int *, int *, int[], int[], int[][MAPWIDTH]);
void backFind(int, int, int *, int *, int [], int [], int [][MAPWIDTH], int *);
void makeMap();
void randomXY(int *, int *, int *);
void PrintText(int, int, char *);
void PrintColorText(int, int, char *, int, int, int);
void PrintShadowedText(int, int, char *, int, int, int);
void clear();
void PutSprite(int, int, int, pngInfo *);


//  PNG画像を読み込むための変数
GLuint imgMapWall; // unsigned int 識別番号
pngInfo infoMapWall; // 構造体 画像の細かい情報(表16.1)
GLuint imgMapRoad;
pngInfo infoMapRoad;

// キャラクター
GLuint imgCharacterUp;
pngInfo infoCharacterUp;
GLuint imgCharacterDown;
pngInfo infoCharacterDown;
GLuint imgCharacterLeft;
pngInfo infoCharacterLeft;
GLuint imgCharacterRight;
pngInfo infoCharacterRight;

// 操作キャラのパラメータ
int characterX = 32;
int characterY = 32;
int characterDirection = DOWN;

// ゴール
GLuint imgGoal;
pngInfo infoGoal;

// 鍵
GLuint imgKey;
pngInfo infoKey;
int keyX, keyY;

// 錠
GLuint imgLock;
pngInfo infoLock;
int lockX, lockY;

// 状態を管理するフラグたち
int mapflag = 0; // マップ生成済みか
int keyflag = 0; // 鍵設置済みか
int getkeyflag = 0; // 鍵取得済みか
int lockflag = 0; // 錠設置済みか
int getlockflag = 0; // 錠取得済みか
int clearflag = 0; // ゲームをクリアしたか

// 迷路の内容を格納する二次元配列
int map[MAPHEIGHT][MAPWIDTH] = {};


int main(int argc, char **argv) {
    //  一般的な準備
    glutInit(&argc, argv);
    glutInitWindowSize((MAPWIDTH - 1) * PARTSIZE, MAPHEIGHT * PARTSIZE);
    glutCreateWindow("Penguin Maze");
    glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // 乱数初期化
    srand(time(NULL));

    //  テクスチャのアルファチャネルを有効にする設定
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    //  PNG画像の読み込み
    imgMapWall = pngBind("./png/mappartsWall.png", PNG_NOMIPMAP, PNG_ALPHA, &infoMapWall, GL_CLAMP, GL_NEAREST, GL_NEAREST);
    imgMapRoad = pngBind("./png/mappartsRoad.png", PNG_NOMIPMAP, PNG_ALPHA, &infoMapRoad, GL_CLAMP, GL_NEAREST, GL_NEAREST);

    imgCharacterUp = pngBind("./png/characterUp.png", PNG_NOMIPMAP, PNG_ALPHA, &infoCharacterUp, GL_CLAMP, GL_NEAREST, GL_NEAREST);
    imgCharacterDown = pngBind("./png/characterDown.png", PNG_NOMIPMAP, PNG_ALPHA, &infoCharacterDown, GL_CLAMP, GL_NEAREST, GL_NEAREST);
    imgCharacterLeft = pngBind("./png/characterLeft.png", PNG_NOMIPMAP, PNG_ALPHA, &infoCharacterLeft, GL_CLAMP, GL_NEAREST, GL_NEAREST);
    imgCharacterRight = pngBind("./png/characterRight.png", PNG_NOMIPMAP, PNG_ALPHA, &infoCharacterRight, GL_CLAMP, GL_NEAREST, GL_NEAREST);

    imgGoal = pngBind("./png/goal.png", PNG_NOMIPMAP, PNG_ALPHA, &infoGoal, GL_CLAMP, GL_NEAREST, GL_NEAREST);
    imgKey = pngBind("./png/key.png", PNG_NOMIPMAP, PNG_ALPHA, &infoKey, GL_CLAMP, GL_NEAREST, GL_NEAREST);
    imgLock = pngBind("./png/lock.png", PNG_NOMIPMAP, PNG_ALPHA, &infoLock, GL_CLAMP, GL_NEAREST, GL_NEAREST);

    //  コールバック関数の登録
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(SpecialKey);

    //  イベントループ突入
    glutMainLoop();

    return(0);
}
//  ウィンドウの表示内容を更新する
void Display(void) {
    int x, y = 0;  //  PNG画像を置く座標
    int i, j;

    glClear(GL_COLOR_BUFFER_BIT);

    if (characterX == GOALX && characterY == GOALY && getlockflag != 0 && clearflag == 0) {
        clearflag = 1;
    } else if (clearflag == 1) {
        clear();
    } else {
        if (mapflag == 0) {
            makeMap();
        }

        for (i = 0; i < MAPHEIGHT; i++) {
            for (j = 0; j < MAPWIDTH - 1; j++) {
                x = PARTSIZE * j;
                y = PARTSIZE * i;
                if (map[i][j] == WALL) {
                    PutSprite(imgMapWall, x, y, &infoMapWall);
                } else if(map[i][j] == ROAD){
                    PutSprite(imgMapRoad, x, y, &infoMapRoad);
                }
            }
        }

        if (getlockflag != 0) {
            PutSprite(imgGoal, GOALX, GOALY, &infoGoal);
        }
        
        if (keyflag == 0) {
            randomXY(&keyX, &keyY, &keyflag);
        }

        if ((abs(characterX - keyX) < PARTSIZE) && (abs(characterY - keyY) < PARTSIZE)) {
            getkeyflag = 1;
        }

        if (getkeyflag == 0) {
            PutSprite(imgKey, keyX, keyY, &infoKey);
        }

        if (lockflag == 0) {
            randomXY(&lockX, &lockY, &lockflag);
        }

        if ((abs(characterX - lockX) < PARTSIZE) && (abs(characterY - lockY) < PARTSIZE) && getkeyflag != 0) {
            getlockflag = 1;
        }

        if (getlockflag == 0) {
            PutSprite(imgLock, lockX, lockY, &infoLock);
        }
    }

    switch (characterDirection) {
        case UP:
            PutSprite(imgCharacterUp, characterX, characterY, &infoCharacterUp);
            break;
        case DOWN:
            PutSprite(imgCharacterDown, characterX, characterY, &infoCharacterDown);
            break;
        case LEFT:
            PutSprite(imgCharacterLeft, characterX, characterY, &infoCharacterLeft);
            break;
        case RIGHT:
            PutSprite(imgCharacterRight, characterX, characterY, &infoCharacterRight);
            break;
    }
    
    glFlush();
}

//  ウィンドウのサイズ変更が発生したときに座標系を再設定する関数
void Reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glScaled(1, -1, 1);
    glTranslated(0, -h, 0);
}

// キー入力の処理
void Keyboard(unsigned char key, int x, int y) {
    if ((key == 'q') || (key == 27)) {
        puts("終了");
        exit(0);
    }
}

// direction 上=0,右=1,下=2,左=3
void SpecialKey(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        characterDirection = UP;
        characterMove(&characterX, &characterY, UP);
        break;
    case GLUT_KEY_DOWN:
        characterDirection = DOWN;
        characterMove(&characterX, &characterY, DOWN);
        break;
    case GLUT_KEY_LEFT:
        characterDirection = LEFT;
        characterMove(&characterX, &characterY, LEFT);
        break;
    case GLUT_KEY_RIGHT:
        characterDirection = RIGHT;
        characterMove(&characterX, &characterY, RIGHT);
        break;
    }

    glutPostRedisplay(); // ディスプレイを更新
}

// direction 上=0,右=1,下=2,左=3
int isMovable(int x, int y, int direction) {
    int cx, cy;
    switch (direction) {
        case UP:
            cx = x / PARTSIZE;
            cy = (y - 1) / PARTSIZE;
            if (x % PARTSIZE == 0) { // キャラクターがマップパーツとぴったり重なる
                if (map[cy][cx] == WALL) {
                    return 0;
                }
            } else {
                if (map[cy][cx] == WALL || map[cy][cx + 1] == WALL) {
                    return 0;
                }
            }
            break;
        case RIGHT:
            cx = x / PARTSIZE;
            cy = y / PARTSIZE;
            if (y % PARTSIZE == 0) { // キャラクターがマップパーツとぴったり重なる
                if (map[cy][cx + 1] == WALL) {
                    return 0;
                }
            } else {
                if (map[cy][cx + 1] == WALL || map[cy + 1][cx + 1] == WALL) {
                    return 0;
                }
            }
            break;
        case DOWN:
            cx = x / PARTSIZE;
            cy = y / PARTSIZE;
            if (x % PARTSIZE == 0) { // キャラクターがマップパーツとぴったり重なる
                if (map[cy + 1][cx] == WALL) {
                    return 0;
                }
            } else {
                if (map[cy + 1][cx] == WALL || map[cy + 1][cx + 1] == WALL) {
                    return 0;
                }
            }
            break;
        case LEFT:
            cx = (x - 1) / PARTSIZE;
            cy = y / PARTSIZE;
            if (y % PARTSIZE == 0) { // キャラクターがマップパーツとぴったり重なる
                if (map[cy][cx] == WALL) {
                    return 0;
                }
            } else {
                if (map[cy][cx] == WALL || map[cy + 1][cx] == WALL) {
                    return 0;
                }
            }
            break;
    }
    return 1;
}


// 移動時に移動できるかを判定し移動
void characterMove(int *x, int *y, int direction) {
    int return_val;

    switch (direction) {
    case UP: // 上へ進む
        return_val = isMovable(*x, *y, UP);
        if (return_val) { // return_valが1のとき実行
            *y -= 8;
        }
        break;
    case DOWN: // 下へ
        return_val = isMovable(*x, *y, DOWN);
        if (return_val) {
            *y += 8;
        }
        break;
    case LEFT: // 左へ
        return_val = isMovable(*x, *y, LEFT);
        if (return_val) {
            *x -= 8;
        }
        break;
    case RIGHT: // 右へ
        return_val = isMovable(*x, *y, RIGHT);
        if (return_val) {
            *x += 8;
        }
        break;
    }
}

// 上に穴を掘る
void digUp(int *i, int *x, int *y, int beforeX[], int beforeY[], int map[][MAPWIDTH]) {
    // 掘る
    map[*y][*x] = ROAD;
    map[*y - 1][*x] = ROAD;
    map[*y - 2][*x] = ROAD;
    beforeX[*i] = *x;
    beforeY[*i] = *y;
    *y -= 2;
    *i += 1;
}

// 下に穴を掘る
void digDown(int *i, int *x, int *y, int beforeX[], int beforeY[], int map[][MAPWIDTH]) {
    map[*y][*x] = ROAD;
    map[*y + 1][*x] = ROAD;
    map[*y + 2][*x] = ROAD;
    beforeX[*i] = *x;
    beforeY[*i] = *y;
    *y += 2;
    *i += 1;
}

// 左に穴を掘る
void digLeft(int *i, int *x, int *y, int beforeX[], int beforeY[], int map[][MAPWIDTH]) {
    map[*y][*x] = ROAD;
    map[*y][*x - 1] = ROAD;
    map[*y][*x - 2] = ROAD;
    beforeX[*i] = *x;
    beforeY[*i] = *y;
    *x -= 2;
    *i += 1;
}

// 右に穴を掘る
void digRight(int *i, int *x, int *y, int beforeX[], int beforeY[], int map[][MAPWIDTH]) {
    map[*y][*x] = ROAD;
    map[*y][*x + 1] = ROAD;
    map[*y][*x + 2] = ROAD;
    beforeX[*i] = *x;
    beforeY[*i] = *y;
    *x += 2;
    *i += 1;
}

// 前の座標に戻って掘れる方向がないか探す
void backFind(int i, int j, int *x, int *y, int beforeX[], int beforeY[], int map[][MAPWIDTH], int *end) {
    while (1) {
        if (i - j < 0) { // マップ作成完了なので終了
            *end = 1;
            break;
        }

        // ひとつ前の座標を読み込む
        *x = beforeX[i - j];
        *y = beforeY[i - j];

        // 掘れる方向を探す
        if (*y - 2 > 0 && map[*y - 2][*x] == WALL) { // 上
            digUp(&i, x, y, beforeX, beforeY, map);
            break;
        } else if (*y + 2 < MAPHEIGHT - 1 && map[*y + 2][*x] == WALL) { // 下
            digDown(&i, x, y, beforeX, beforeY, map);
            break;
        } else if (*x - 2 > 0 && map[*y][*x - 2] == WALL) { // 左
            digLeft(&i, x, y, beforeX, beforeY, map);
            break;
        } else if (*x + 2 < MAPWIDTH - 2 && map[*y][*x + 2] == WALL) { // 右
            digRight(&i, x, y, beforeX, beforeY, map);
            break;
        } else { // 掘れない
            j++; // さらにもどる
        }
    }
}

// マップ作成(穴掘り法)
void makeMap() {
    int i, j;
    int x = 0;
    int y = 0;
    int direction = 10; // 方向以外の数字
    int r;
    int end = 0;

    int beforeX[MAPWIDTH * MAPHEIGHT];
    int beforeY[MAPWIDTH * MAPHEIGHT];

    for (i = 0; i < MAPWIDTH * MAPHEIGHT; i++) {
        beforeX[i] = 0;
    }

    for (i = 0; i < MAPWIDTH * MAPHEIGHT; i++) {
        beforeY[i] = 0;
    }

    // キャラクターの初期位置
    characterX = 32;
    characterY = 32;

    // すべてを壁として初期化
    for (i = 0; i <= MAPHEIGHT; i++) {
        for (j = 0; j <= MAPWIDTH; j++) {
            map[i][j] = WALL;   
        }
    }

    // 最初の座標を決定
    while (1) {
        r = rand() % (MAPHEIGHT - 2) + 1; // 配列の添字で1から9の範囲の乱数生成
        if (r % 2 == 1 && x == 0) { // 偶数(配列の添字は奇数)
            /* -1した値が配列で指定する際の座標 */
            x = r;
        } else if (r % 2 == 1 && x != 0) {
            y = r;
            break;
        }
    }

    i = 0;
    while (1) {
        r = rand() % 4; // 0～3の乱数生成

        // 掘る方向をランダムに決める    
        switch (r) {
            case 0: // 上
                direction = UP;
                break;
            case 1: // 下
                direction = DOWN;
                break;
            case 2: // 左
                direction = LEFT;
                break;
            case 3: // 右
                direction = RIGHT;
                break;
        }


        // 決めた向きで掘れるかどうか確認
        switch (direction) {
            case UP: // 上
                // 掘れる
                if (y - 2 > 0 && map[y - 2][x] == WALL) {
                    // 掘る
                    digUp(&i, &x, &y, beforeX, beforeY, map);
                    break;
                } else { // 現在地で決めた方向では掘れなかった
                    // まず現在地で他に掘れる方向がないか見る
                    if (y + 2 < MAPHEIGHT - 1 && map[y + 2][x] == WALL) { // 下
                        digDown(&i, &x, &y, beforeX, beforeY, map);
                        break;
                    } else if (x - 2 > 0 && map[y][x - 2] == WALL) { // 左
                        digLeft(&i, &x, &y, beforeX, beforeY, map);
                        break;
                    } else if (x + 2 < MAPWIDTH - 2 && map[y][x + 2] == WALL) { // 右
                        digRight(&i, &x, &y, beforeX, beforeY, map);
                        break;
                    } else { // 現在地では掘れない
                        // 戻って掘れる場所があるか探す
                        j = 1;
                        backFind(i, j, &x, &y, beforeX, beforeY, map, &end);
                        break;
                    }
                }
            case DOWN: // 下
                // 掘れる
                if (y + 2 < MAPHEIGHT - 1 && map[y + 2][x] == WALL) {
                    digDown(&i, &x, &y, beforeX, beforeY, map);
                    break;
                } else { // 現在地で決めた方向では掘れなかった
                    // まず現在地で他に掘れる方向がないか見る
                    if (y - 2 > 0 && map[y - 2][x] == WALL) { // 上
                        digUp(&i, &x, &y, beforeX, beforeY, map);
                        break;
                    } else if (x - 2 > 0 && map[y][x - 2] == WALL) { // 左
                        digLeft(&i, &x, &y, beforeX, beforeY, map);
                        break;
                    } else if (x + 2 < MAPWIDTH - 2 && map[y][x + 2] == WALL) { // 右
                        digRight(&i, &x, &y, beforeX, beforeY, map);
                        break;
                    } else { // 現在地では掘れない
                        j = 1;
                        backFind(i, j, &x, &y, beforeX, beforeY, map, &end);
                        break;
                    }
                }
            case LEFT: // 左
                if (x - 2 > 0 && map[y][x - 2] == WALL) {
                    digLeft(&i, &x, &y, beforeX, beforeY, map);
                    break;
                } else { // 現在地で決めた方向では掘れなかった
                    // まず現在地で他に掘れる方向がないか見る
                    if (y - 2 > 0 && map[y - 2][x] == WALL) { // 上
                        digUp(&i, &x, &y, beforeX, beforeY, map);
                        break;
                    } else if (y + 2 < MAPHEIGHT - 1 && map[y + 2][x] == WALL) { // 下
                        digDown(&i, &x, &y, beforeX, beforeY, map);
                        break;
                    } else if (x + 2 < MAPWIDTH - 2 && map[y][x + 2] == WALL) { // 右
                        digRight(&i, &x, &y, beforeX, beforeY, map);
                        break;
                    } else { // 現在地では掘れない
                        j = 1;
                        backFind(i, j, &x, &y, beforeX, beforeY, map, &end);
                        break;
                    }
                }
            case RIGHT: // 右
                if (x + 2 < MAPWIDTH - 2 && map[y][x + 2] == WALL) {
                    digRight(&i, &x, &y, beforeX, beforeY, map);
                    break;
                } else {  // 現在地で決めた方向では掘れなかった
                    // まず現在地で他に掘れる方向がないか見る
                    if (y - 2 > 0 && map[y - 2][x] == WALL) { // 上
                        digUp(&i, &x, &y, beforeX, beforeY, map);
                        break;
                    } else if (y + 2 < MAPHEIGHT - 1 && map[y + 2][x] == WALL) { // 下
                        digDown(&i, &x, &y, beforeX, beforeY, map);
                        break;
                    } else if (x - 2 > 0 && map[y][x - 2] == WALL) { // 左
                        digLeft(&i, &x, &y, beforeX, beforeY, map);
                        break;
                    } else { // 現在地では掘れない
                        j = 1;
                        backFind(i, j, &x, &y, beforeX, beforeY, map, &end);
                        break;
                    }
                }   
        }

        if (end == 1) { // マップ生成完了
            break;
        }
    }
    
    puts("マップ作成完了");
    mapflag = 1;    
}

// ランダムにXY座標を決める
void randomXY(int *x, int *y, int *flag) {
    int r;

    while (1) {
        r = rand() % MAPHEIGHT; // 0～10の乱数生成

        if (r != 1 && r != 9 && map[r][r] == ROAD && keyX != r * PARTSIZE) {
            *x = r * PARTSIZE;
            *y = r * PARTSIZE;
            *flag = 1;
            break;
        }
    }
}

// 指定した座標に文字を描画
void PrintText(int x, int y, char *str) {
    int i;

    glRasterPos2i(x, y);
    for (i = 0; i <= strlen(str); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
    }
}

// 指定した色で文字を描画
void PrintColorText(int x, int y, char *str, int r, int g, int b) {
    glColor3ub(r, g, b);
    PrintText(x, y, str);
}

// 影付きで文字を描画
void PrintShadowedText(int x, int y, char *str, int r, int g, int b) {
    // 影を描画(黒い画面に書くことを想定)
    PrintColorText(x + 2, y + 2, str, 77, 77, 77);
    PrintColorText(x + 1, y + 1, str, 77, 77, 77);
    // 太文字で文字を描画
    PrintColorText(x, y, str, r, g, b);
    PrintColorText(x - 1, y, str, r, g, b);
}

// クリア画面を生成
void clear() {
    int wdiv10 = MAPWIDTH / 10;
    int wdiv100 = wdiv10 / 10;
    int i, j;
    for (i = 0; i < MAPHEIGHT; i++) {
        for (j = 0; j < MAPWIDTH; j++) {
            if (i == 0 || i == MAPHEIGHT - 1 || j == 0 || j == MAPWIDTH - 2) {
                map[i][j] = WALL;
            } else {
                map[i][j] = ROAD;
            }
        }
    }

    PrintShadowedText(32 * wdiv10 * 4 + wdiv100 * 90, 32 * MAPHEIGHT / 2, "CLEAR", 255, 255, 255);
    PrintShadowedText(32 * wdiv10 * 1 + wdiv100 * 90, 32 * MAPHEIGHT / 10 * 6, "Press q or Esc key to exit", 255, 255, 255);
}

//  num番のPNG画像を座標(x,y)に表示する
void PutSprite(int num, int x, int y, pngInfo *info_map) {
    int w, h;  //  テクスチャの幅と高さ


    w = info_map->Width;   //  テクスチャの幅と高さを取得する
    h = info_map->Height;

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, num);
    glColor4ub(255, 255, 255, 255);

    glBegin(GL_QUADS);  //  幅w, 高さhの四角形

    // p145に解説
    glTexCoord2i(0, 0); 
    glVertex2i(x, y);

    glTexCoord2i(0, 1);
    glVertex2i(x, y + h);

    glTexCoord2i(1, 1);
    glVertex2i(x + w, y + h);

    glTexCoord2i(1, 0);
    glVertex2i(x + w, y);

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}
