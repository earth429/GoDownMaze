#include <GL/glut.h>
#include <GL/glpng.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PARTS_NUM 100

void Display(void);
void Reshape(int, int);
//void Timer(int);
//void EnemyTimer1(int);
//void EnemyTimer2(int);
void Keyboard(unsigned char, int, int);
void SpecialKey(int, int, int);
//int isMovable(int, int, int);
void PutSprite(int, int, int, pngInfo *);


//  PNG画像を読み込むための変数
GLuint img_mapA; // unsigned int 識別番号
pngInfo info_mapA; // 構造体 画像の細かい情報(表16.1)
GLuint img_mapB; // unsigned int 識別番号
pngInfo info_mapB; // 構造体 画像の細かい情報(表16.1)

// キャラクター
GLuint img_character; // unsigned int 識別番号
pngInfo info_character; // 構造体 画像の細かい情報(表16.1)

int characterX = 32;
int characterY = 32;

// 敵
GLuint img_enemy1; // unsigned int 識別番号
pngInfo info_enemy1; // 構造体 画像の細かい情報(表16.1)
GLuint img_enemy2; // unsigned int 識別番号
pngInfo info_enemy2; // 構造体 画像の細かい情報(表16.1)

int enemy1X = 256;
int enemy1Y = 32;
int enemy2X = 256;
int enemy2Y = 256;

// 線を引くための
#define WIDTH 160
#define HEIGHT 160
int point[WIDTH][HEIGHT];

#define WHITE 0
#define BLACK 1



// p169
char map[10][11] = {
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
};

int main(int argc, char **argv) {
    //  一般的な準備
    srandom(12345);
    glutInit(&argc, argv);
    //glutInitWindowSize(320, 320);
    glutInitWindowSize(WIDTH * 2, HEIGHT * 2);
    glutCreateWindow("qix");
    glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    // 乱数用
    srand(time(NULL));

    //  テクスチャのアルファチャネルを有効にする設定
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    //  PNG画像の読み込み

    //img_mapA = pngBind("./png/mapparts1.png", PNG_NOMIPMAP, PNG_ALPHA,
                         //&info_mapA, GL_CLAMP, GL_NEAREST, GL_NEAREST);

    //img_mapB = pngBind("./png/mapparts2.png", PNG_NOMIPMAP, PNG_ALPHA,
                         //&info_mapB, GL_CLAMP, GL_NEAREST, GL_NEAREST);

    img_character = pngBind("./png/character.png", PNG_NOMIPMAP, PNG_ALPHA,
                         &info_character, GL_CLAMP, GL_NEAREST, GL_NEAREST);

    //img_enemy1 = pngBind("./png/enemy1.png", PNG_NOMIPMAP, PNG_ALPHA,
                         //&info_enemy1, GL_CLAMP, GL_NEAREST, GL_NEAREST);

    //img_enemy2 = pngBind("./png/enemy2.png", PNG_NOMIPMAP, PNG_ALPHA,
                         //&info_enemy2, GL_CLAMP, GL_NEAREST, GL_NEAREST);


    //  コールバック関数の登録
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    //glutTimerFunc(100, Timer, 0);
    //glutTimerFunc(100, EnemyTimer1, 0);
    //glutTimerFunc(100, EnemyTimer2, 0);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(SpecialKey);

    //  イベントループ突入
    glutMainLoop();

    return(0);
}


//  タイマー割り込みで画面を描画し直す
/*void Timer(int t)
{
    glutPostRedisplay();
    glutTimerFunc(100, Timer, 0);
}*/



//  ウィンドウのサイズ変更が発生したときに座標系を再設定する関数
void Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glScaled(1, -1, 1);
    glTranslated(0, -h, 0);
}


//  ウィンドウの表示内容を更新する
void Display(void)
{
    //int x, y = 0;  //  PNG画像を置く座標
    int i, j, p;

    glClear(GL_COLOR_BUFFER_BIT);

    //glColor3ub(0, 0, 0);
    //glColor3ub(255, 255, 255);
    for (i = 0; i < WIDTH; i++) {
        for (j = 0; j < HEIGHT; j++) {
            p = point[i][j];
            if (p == WHITE) {
                glColor3ub(255, 255, 255);
            }
            if (p == BLACK) {
                glColor3ub(0, 0, 0);
            }
            glBegin(GL_POINTS);
            glVertex2i(i * 2, j * 2);
            glVertex2i(i * 2 + 1, j * 2);
            glVertex2i(i * 2, j * 2 + 1);
            glVertex2i(i * 2 + 1, j * 2 + 1);
            glEnd();
        }   
    }


    PutSprite(img_character, characterX, characterY, &info_character);

    //PutSprite(img_enemy1, enemy1X, enemy1Y, &info_enemy1);
    //PutSprite(img_enemy2, enemy2X, enemy2Y, &info_enemy2);

    /*glBegin(GL_LINES);
            glVertex2i(characterX, characterY);
            characterY -= 8;
            glVertex2i(characterX, characterY);
    glEnd();*/

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
    int return_val=1;
    switch (key)
    {
    case GLUT_KEY_UP:
        //return_val = isMovable(characterX, characterY, 0);
        if (return_val)
        { // return_valが1のとき実行
            //printf("%d, %d\n", characterX, characterY);
            point[characterX / 2][characterY / 2] = BLACK;

            characterY -= 8;
            //puts("上");
            //printf("%d, %d\n", characterX, characterY);
            point[characterX / 2][characterY / 2] = BLACK;
        }
        break;
    case GLUT_KEY_DOWN:
        //return_val = isMovable(characterX, characterY, 2);
        if (return_val)
        { // return_valが1のとき実行
            point[characterX / 2][characterY / 2] = BLACK;
            characterY += 8;
            point[characterX / 2][characterY / 2] = BLACK;
            //puts("下");
        }
        break;
    case GLUT_KEY_LEFT:
        //return_val = isMovable(characterX, characterY, 3);
        if (return_val)
        { // return_valが1のとき実行
            point[characterX / 2][characterY / 2] = BLACK;
            characterX -= 8;
            point[characterX / 2][characterY / 2] = BLACK;
            //puts("左");
        }
        break;
    case GLUT_KEY_RIGHT:
        //return_val = isMovable(characterX, characterY, 1);
        if (return_val)
        { // return_valが1のとき実行
            point[characterX / 2][characterY / 2] = BLACK;
            characterX += 8;
            point[characterX / 2][characterY / 2] = BLACK;
            //puts("右");
        }
        break;
    default:
        break;
        }
    glutPostRedisplay(); // ディスプレイを更新
    
}

// direction 上=0,右=1,下=2,左=3
/*int isMovable(int x, int y, int direction) {
    int cx, cy;
    switch (direction) {
        case 0:
            cx = characterX / 32;
            cy = (characterY - 1) / 32;
            if (characterX % 32 == 0) { // キャラクターがマップパーツとぴったり重なる
                if (map[cy][cx] == 'A') { // Aは壁
                    return 0;
                }
            } else {
                if (map[cy][cx] == 'A' || map[cy][cx + 1] == 'A') { // Aは壁
                    return 0;
                }
            }
            break;
        case 1:
            cx = characterX / 32;
            cy = characterY / 32;
            if (characterY % 32 == 0) { // キャラクターがマップパーツとぴったり重なる
                if (map[cy][cx + 1] == 'A') { // Aは壁
                    return 0;
                }
            } else {
                if (map[cy][cx + 1] == 'A' || map[cy + 1][cx + 1] == 'A') { // Aは壁
                    return 0;
                }
            }
            break;
        case 2:
            cx = characterX / 32;
            cy = characterY / 32;
            if (characterX % 32 == 0) { // キャラクターがマップパーツとぴったり重なる
                if (map[cy + 1][cx] == 'A') { // Aは壁
                    return 0;
                }
            } else {
                if (map[cy + 1][cx] == 'A' || map[cy + 1][cx + 1] == 'A') { // Aは壁
                    return 0;
                }
            }
            break;
        case 3:
            cx = (characterX - 1) / 32;
            cy = characterY / 32;
            if (characterY % 32 == 0) { // キャラクターがマップパーツとぴったり重なる
                if (map[cy][cx] == 'A') { // Aは壁
                    return 0;
                }
            } else {
                if (map[cy][cx] == 'A' || map[cy + 1][cx] == 'A') { // Aは壁
                    return 0;
                }
            }
            break;
        default:
            // とくになし
            break;
    }
    return 1;
}*/

//
//  num番のPNG画像を座標(x,y)に表示する
//
void PutSprite(int num, int x, int y, pngInfo *info_map)
{
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
