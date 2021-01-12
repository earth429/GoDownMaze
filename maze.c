#include <GL/glut.h>
#include <GL/glpng.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PARTS_NUM 100

void Display(void);
void Reshape(int, int);
void Timer(int);
void EnemyTimer1(int);
void EnemyTimer2(int);
void Keyboard(unsigned char, int, int);
void SpecialKey(int, int, int);
int isMovable(int, int, int);
void CharacterMove(int *, int *, int);
void clear();
void makeMap();
void PutSprite(int, int, int, pngInfo *);


//  PNG画像を読み込むための変数
GLuint img_mapA; // unsigned int 識別番号
pngInfo info_mapA; // 構造体 画像の細かい情報(表16.1)
GLuint img_mapB; // unsigned int 識別番号
pngInfo info_mapB; // 構造体 画像の細かい情報(表16.1)

// キャラクター
GLuint img_character; // unsigned int 識別番号
pngInfo info_character; // 構造体 画像の細かい情報(表16.1)

// 操作キャラのパラメータ
int characterX = 32;
int characterY = 32;
int life = 50;

int enemy1X = 256;
int enemy1Y = 32;
int enemy2X = 256;
int enemy2Y = 256;


// 敵
GLuint img_enemy1; // unsigned int 識別番号
pngInfo info_enemy1; // 構造体 画像の細かい情報(表16.1)
GLuint img_enemy2; // unsigned int 識別番号
pngInfo info_enemy2; // 構造体 画像の細かい情報(表16.1)

int gameflag = 0;

#define MAPWIDTH 12
#define MAPHEIGHT 11

#define WALL 0
#define ROAD 1

// direction 上=0,右=1,下=2,左=3
#define UP 0
#define DOWN 2
#define LEFT 3
#define RIGHT 1

int map[MAPHEIGHT][MAPWIDTH] = {};
// p169
/*char map[MAPHEIGHT][MAPWIDTH] = {
    "AAAAAAAAAA",
    "ABBBBBBBBA",
    "ABBBAABBBA",
    "ABBBBBBBBA",
    "ABBBAAAAAA",
    "ABABBBBBBA",
    "AAABBAABBA",
    "ABBBBAAABA",
    "ABBBBBBBBA",
    "AAAAAAAAAA",
};*/

int main(int argc, char **argv) {
    //  一般的な準備
    srandom(12345);
    glutInit(&argc, argv);
    glutInitWindowSize((MAPWIDTH - 1) * 32, MAPHEIGHT * 32); //320
    glutCreateWindow("GoDownMaze");
    glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA);
    glClearColor(0.0, 0.0, 1.0, 1.0);

    // 乱数用
    srand(time(NULL));

    //  テクスチャのアルファチャネルを有効にする設定
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    //  PNG画像の読み込み

    img_mapA = pngBind("./png/mapparts1.png", PNG_NOMIPMAP, PNG_ALPHA, &info_mapA, GL_CLAMP, GL_NEAREST, GL_NEAREST);

    img_mapB = pngBind("./png/mapparts2.png", PNG_NOMIPMAP, PNG_ALPHA, &info_mapB, GL_CLAMP, GL_NEAREST, GL_NEAREST);

    img_character = pngBind("./png/character.png", PNG_NOMIPMAP, PNG_ALPHA, &info_character, GL_CLAMP, GL_NEAREST, GL_NEAREST);

    img_enemy1 = pngBind("./png/enemy1.png", PNG_NOMIPMAP, PNG_ALPHA, &info_enemy1, GL_CLAMP, GL_NEAREST, GL_NEAREST);

    img_enemy2 = pngBind("./png/enemy2.png", PNG_NOMIPMAP, PNG_ALPHA, &info_enemy2, GL_CLAMP, GL_NEAREST, GL_NEAREST);


    //  コールバック関数の登録
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutTimerFunc(100, Timer, 0);
    glutTimerFunc(100, EnemyTimer1, 0);
    glutTimerFunc(100, EnemyTimer2, 0);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(SpecialKey);

    //  イベントループ突入
    glutMainLoop();

    return(0);
}



// 移動時に移動できるかを判定し移動
void CharacterMove(int *x, int *y, int direction) {
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
    default: // なにもしない
        break;
    }
}


//  タイマー割り込みで画面を描画し直す
void Timer(int t) {
    glutPostRedisplay();
    glutTimerFunc(100, Timer, 0);
}

//  タイマー割り込みで画面を描画し直す(敵1用)
void EnemyTimer1(int t) {   
    int r;
    r = rand() % 10;
    //printf("r:%d\n", r);
    
    switch (r) {
        case 0: // 上
            CharacterMove(&enemy1X, &enemy1Y, UP);
            break;
        case 1: // 下
            CharacterMove(&enemy1X, &enemy1Y, DOWN);
            break;
        case 2: // 左
            CharacterMove(&enemy1X, &enemy1Y, LEFT);
            break;
        case 3: // 右
            CharacterMove(&enemy1X, &enemy1Y, RIGHT);
            break;
        default:
            break;
    }

    
    // 当たり判定
    if ((abs(characterX - enemy1X) < 32) && (abs(characterY - enemy1Y) < 32)) {
        //double startTime, endTime;
        //double totalTime = 0.0;
        //double setTime = 3.0;

        puts("enemy1:Hit");
        life--;
        printf("life:%d", life);
        if (life <= 0) {
            puts("ゲームオーバー");
            exit(0);
        }

        /*time_t last = time(0);
        time_t next;
        int pastSec = 0;

        while(1) {
        if (time(&next) != last) { 
            last = next;
            pastSec++;
            printf("%d 秒経過\n", pastSec);
            if (pastSec == 3) {
                break;
            }
        }
        }*/
        /*startTime = clock() / CLOCKS_PER_SEC;

        while(1) {
            if (totalTime > setTime) {
                break;
            }

            endTime = clock() / CLOCKS_PER_SEC;
            totalTime = endTime - startTime;
        }*/
    }

    glutPostRedisplay();
    glutTimerFunc(100, EnemyTimer1, 0);
}

//  タイマー割り込みで画面を描画し直す(敵2用)
void EnemyTimer2(int t) {
    int r;
    r = rand() % 10;
    //printf("r:%d\n", r);
    
    switch (r) {
        case 0: // 上
            CharacterMove(&enemy2X, &enemy2Y, UP);
            break;
        case 1: // 下
            CharacterMove(&enemy2X, &enemy2Y, DOWN);
            break;
        case 2: // 左
            CharacterMove(&enemy2X, &enemy2Y, LEFT);
            break;
        case 3: // 右
            CharacterMove(&enemy2X, &enemy2Y, RIGHT);
            break;
        default:
            break;
    }
    
    // 当たり判定
    if ((abs(characterX - enemy2X) < 32) && (abs(characterY - enemy2Y) < 32)) {
        puts("enemy2:Hit");
        life--;
        printf("life:%d", life);
        if (life <= 0) {
            puts("ゲームオーバー");
            exit(0);
        }

    }
    
    glutPostRedisplay();
    glutTimerFunc(100, EnemyTimer2, 0);
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


//  ウィンドウの表示内容を更新する
void Display(void) {
    int x, y = 0;  //  PNG画像を置く座標
    int i, j;

    glClear(GL_COLOR_BUFFER_BIT);

    if (gameflag == 1) {
        makeMap();
        gameflag++;
    }
    makeMap();

    for (i = 0; i < MAPHEIGHT; i++) {
        for (j = 0; j < MAPWIDTH - 1; j++) {
            x = 32 * j;
            y = 32 * i;
            if (map[i][j] == WALL) {
                PutSprite(img_mapA, x, y, &info_mapA);
            } else if(map[i][j] == ROAD){
                PutSprite(img_mapB, x, y, &info_mapB);
            }
        }
    }


    PutSprite(img_character, characterX, characterY, &info_character);

    if (characterX == 256 && characterY == 256) {
        clear();
    }

    PutSprite(img_enemy1, enemy1X, enemy1Y, &info_enemy1);
    PutSprite(img_enemy2, enemy2X, enemy2Y, &info_enemy2);

    glFlush();
}


// キー入力の処理
void Keyboard(unsigned char key, int x, int y) {
    if ((key == 'q') || (key == 27)) {
        puts("終了");
        exit(0);
    }
}

// p165 特殊キー
// direction 上=0,右=1,下=2,左=3
void SpecialKey(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        CharacterMove(&characterX, &characterY, UP);
        break;
    case GLUT_KEY_DOWN:
        CharacterMove(&characterX, &characterY, DOWN);
        break;
    case GLUT_KEY_LEFT:
        CharacterMove(&characterX, &characterY, LEFT);
        break;
    case GLUT_KEY_RIGHT:
        CharacterMove(&characterX, &characterY, RIGHT);
        break;
    default:
        break;
    }

    glutPostRedisplay(); // ディスプレイを更新
}

// direction 上=0,右=1,下=2,左=3
int isMovable(int x, int y, int direction) {
    int cx, cy;
    switch (direction) {
        case 0:
            cx = x / 32;
            cy = (y - 1) / 32;
            if (x % 32 == 0) { // キャラクターがマップパーツとぴったり重なる
                if (map[cy][cx] == WALL) { // Aは壁
                    return 0;
                }
            } else {
                if (map[cy][cx] == WALL || map[cy][cx + 1] == WALL) { // Aは壁
                    return 0;
                }
            }
            break;
        case 1:
            cx = x / 32;
            cy = y / 32;
            if (y % 32 == 0) { // キャラクターがマップパーツとぴったり重なる
                if (map[cy][cx + 1] == WALL) { // Aは壁
                    return 0;
                }
            } else {
                if (map[cy][cx + 1] == WALL || map[cy + 1][cx + 1] == WALL) { // Aは壁
                    return 0;
                }
            }
            break;
        case 2:
            cx = x / 32;
            cy = y / 32;
            if (x % 32 == 0) { // キャラクターがマップパーツとぴったり重なる
                if (map[cy + 1][cx] == WALL) { // Aは壁
                    return 0;
                }
            } else {
                if (map[cy + 1][cx] == WALL || map[cy + 1][cx + 1] == WALL) { // Aは壁
                    return 0;
                }
            }
            break;
        case 3:
            cx = (x - 1) / 32;
            cy = y / 32;
            if (y % 32 == 0) { // キャラクターがマップパーツとぴったり重なる
                if (map[cy][cx] == WALL) { // Aは壁
                    return 0;
                }
            } else {
                if (map[cy][cx] == WALL || map[cy + 1][cx] == WALL) { // Aは壁
                    return 0;
                }
            }
            break;
        default:
            // とくになし
            break;
    }
    return 1;
}

void clear() {
    //puts("aho");
    //int i;
    puts("クリア");
    gameflag++;

    //exit(0);
    //puts("ahoaho");
}

// マップ作成(穴掘り法)
void makeMap() {
    int i, j;
    int x = 0;
    int y = 0;
    int direction = 10; // 方向以外の数字
    int r;

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
        r = rand() % (MAPHEIGHT - 3) + 2; // 2から8の範囲の乱数生成
        if (r % 2 == 0 && x == 0) { // 奇数(配列の添字は偶数)
            /* -1した値が配列で指定する際の座標 */
            x = r;
        } else if (r % 2 == 0 && x != 0) {
            y = r;
            break;
        }
        
        //sleep(1);
    }
    printf("x:%d, y:%d\n", x, y);

    i = 0;
    while (1) {
        r = rand() % 10;

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
            default:
                continue; // 決め直し
                //break;
        }


        // 決めた向きで掘れるかどうか確認
        switch (direction) {
            case UP:
                // 掘れる
                if (y - 2 > 0 && map[y - 2][x] == WALL) {
                    // 掘る
                    map[y][x] = ROAD;
                    map[y - 1][x] = ROAD;
                    map[y - 2][x] = ROAD;
                    y -= 2;
                    puts("上");
                    break;
                } else {
                    // とりあえずなにもしない
                    break;
                }
            case DOWN:
                if (y + 2 < MAPHEIGHT - 1 && map[y + 2][x] == WALL) {
                    map[y][x] = ROAD;
                    map[y + 1][x] = ROAD;
                    map[y + 2][x] = ROAD;
                    y += 2;
                    puts("下");
                    break;
                } else {
                    // とりあえずなにもしない
                    break;
                }
            case LEFT:
                if (x - 2 > 0 && map[y][x - 2] == WALL) {
                    map[y][x] = ROAD;
                    map[y][x - 1] = ROAD;
                    map[y][x - 2] = ROAD;
                    x -= 2;
                    puts("左");
                    break;
                } else {
                    // とりあえずなにもしない
                    break;
                }
            case RIGHT: 
                if (x + 2 < MAPWIDTH - 2 && map[y][x + 2] == WALL) {
                    map[y][x] = ROAD;
                    map[y][x + 1] = ROAD;
                    map[y][x + 2] = ROAD;
                    x += 2;
                    puts("右");
                    break;
                } else {
                    // とりあえずなにもしない
                    break;
                }   
        }
        if (i == 20) {
            break;
        }
        i++;
        //puts("堀ループ");
    }
    
    







    //enemy1X = 256;
    //enemy1Y = 32;
    //enemy2X = 256;
    //enemy2Y = 256;

    /*for (i = 0; i < MAPWIDTH; i++) {
        for (j = 0; j < MAPHEIGHT; j++) {
            r = rand() % 2;
            if (i == 0 || i == 9 || j == 0 || j == 9) {
                map[i][j] = 'A';
            } else {
                r = rand() % 2;
                if (r == 0) {
                    map[i][j] = 'B';
                } else {
                    map[i][j] = 'A';
                }
            }
            
        }
    }
    
    map[10][11] = {
        "AAAAAAAAAA",
        "ABBBBBBBBA",
        "ABBBAABBBA",
        "ABBBBBBBBA",
        "ABBBAAAAAA",
        "ABABBBBBBA",
        "AAABBAABBA",
        "ABBBBAAABA",
        "ABBBBBBBBA",
        "AAAAAAAAAA",
    };*/

    puts("マップ作成完了");
    sleep(2);
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
