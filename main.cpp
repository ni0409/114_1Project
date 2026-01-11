#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

#define RLUTIL_USE_ANSI
#include "rlutil.h"

#include <locale> // for UTF-8

using namespace std;

// 小工具：逐字輸出（可選）
void typePrint(const string& s, int ms = 12) {
    for (char c : s) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(ms));
    }
}

// 小工具：等待按鍵（Enter / 任意鍵）
void waitAnyKey(const string& hint = u8"按任意鍵繼續...") {
    cout << "\n\n" << hint << flush;
    rlutil::anykey();
}

int readKey() {
    return rlutil::getkey();
}

int menuSelect(const std::vector<std::string>& options,
               const std::string& title = "") {
    int idx = 0;

    while (true) {
        rlutil::cls();

        if (!title.empty()) {
            rlutil::setColor(rlutil::CYAN);
            std::cout << title << "\n\n";
        }

        for (int i = 0; i < (int)options.size(); i++) {
            if (i == idx) {
                rlutil::setBackgroundColor(rlutil::BLUE);
                rlutil::setColor(rlutil::WHITE);
                std::cout << "▶ " << options[i] << "\n";
                rlutil::setBackgroundColor(rlutil::BLACK);
            } else {
                rlutil::setColor(rlutil::GREY);
                std::cout << "  " << options[i] << "\n";
            }
        }

        std::cout.flush();

        int key = readKey();

        if (key == rlutil::KEY_UP) {
            idx = (idx - 1 + options.size()) % options.size();
        } else if (key == rlutil::KEY_DOWN) {
            idx = (idx + 1) % options.size();
        } else if (key == rlutil::KEY_ENTER || key == '\n' || key == '\r') {
            return idx;
        } else if (key == rlutil::KEY_ESCAPE) {
            return options.size() - 1;
        }
    }
}

// 場景（狀態機）
enum class Scene {
    Intro,
    Room,
    Talk,
    Door,
    Classroom1,
    Classroom2,
    Classroom3,
    Dorm1,
    Test,
    EndingBad1,
    EndingBad2,
    EndingBad3,
    EndingGood,
    EndingNormal1,
    TrueEnding,
    Quit
};

int main() {

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // 遊戲狀態變數（專題加分點）
    int roommate = 0;
    bool hasKey = false;
    int study = 0;
    int sign = 0;

    Scene scene = Scene::Intro;

    while (scene != Scene::Quit) {
        rlutil::cls();
        rlutil::setColor(rlutil::CYAN);

        if (scene == Scene::Intro) {
            rlutil::locate(4, 2);
            cout << u8"《終端機對話冒險 Demo》\n";
            rlutil::setColor(rlutil::GREY);
            rlutil::locate(4, 4);
            typePrint(u8"早晨，你在陌生的床上醒來，想起今天是你來到中原大學的第一天。\n");
            rlutil::locate(4, 5);
            typePrint(u8"\n你低頭一看，室友正在座位上收拾包包。\n");

            std::vector<std::string> opt = {u8"開始", u8"離開"};
            int pick = menuSelect(opt, u8"早晨，你在陌生的床上醒來，想起今天是你來到中原大學的第一天。\n你低頭一看，室友正在座位上收拾包包。");
            scene = (pick == 0) ? Scene::Room : Scene::Quit;
        }

        else if (scene == Scene::Room) {
            rlutil::locate(4, 0);
            rlutil::setColor(rlutil::YELLOW);
            cout << u8"宿舍\n";
            rlutil::setColor(rlutil::GREY);

            rlutil::locate(4, 4);
            std::vector<std::string> opt = {u8"查看桌子", u8"和室友搭話", u8"出門上學", u8"離開遊戲"};
            int pick = menuSelect(opt, u8"房間裡有：你的書桌、床、以及你的室友。");

            if (pick == 0) {
                rlutil::cls();
                rlutil::locate(4, 0);
                rlutil::setColor(rlutil::GREEN);
                cout << u8"桌子\n";
                rlutil::setColor(rlutil::GREY);
                rlutil::locate(4, 4);

                if (!hasKey) {
                    typePrint(u8"你在桌子抽屜裡找到你的學生證。\n");
                    hasKey = true;
                } else {
                    typePrint(u8"桌子已經被你翻過了，沒有新東西。\n");
                }
                waitAnyKey();
                scene = Scene::Room;
            } else if (pick == 1) {
                scene = Scene::Talk;
            } else if (pick == 2) {
                scene = Scene::Door;
            } else {
                scene = Scene::Quit;
            }
        }

        else if (scene == Scene::Talk) {
            rlutil::locate(4, 0);
            rlutil::setColor(rlutil::MAGENTA);
            cout << u8"對話：室友\n";
            rlutil::setColor(rlutil::GREY);

            rlutil::locate(4, 4);

            std::vector<std::string> opt = {u8"嗯，我正要去洗潄。", u8"沒有，我打算翹課。", u8"默不作聲"};
            int pick = menuSelect(opt, u8"『早安，』室友說，『你要去上課了嗎？』\n");


            rlutil::cls();
            rlutil::locate(4, 0);
            rlutil::setColor(rlutil::MAGENTA);
            cout << u8"對話結果\n\n";
            rlutil::setColor(rlutil::GREY);
            rlutil::locate(4, 4);

            if (pick == 0) {
                typePrint(u8"室友點點頭：『那我們一起過去吧。記得帶學生證！』\n");
                roommate += 1;
            } else if (pick == 1) {
                typePrint(u8"室友皺眉：『這才開學第一天耶。』\n");
                roommate -= 1;
            } else {
                typePrint("室友聳聳肩，回過頭繼續收拾包包了。\n");
                // roommate 不變
            }

            rlutil::locate(4, 6);
            cout << u8"\n（室友好感度 = " << roommate << u8"）\n";
            waitAnyKey();
            scene = Scene::Room;
        }

        else if (scene == Scene::Door) {
            rlutil::locate(4, 0);
            rlutil::setColor(rlutil::YELLOW);
            cout << u8"門\n";
            rlutil::setColor(rlutil::GREY);

            rlutil::locate(4, 4);
            if (!hasKey) {
                typePrint(u8"上課時間快到了，你和室友一起下樓，卻發現沒帶學生證，無法出去。\n");
                typePrint(u8"傳達室的學姊問：『你沒帶學生證嗎？』\n");

                std::vector<std::string> opt = {
                    u8"跑回去拿學生證",
                    u8"跟在室友後面直接出去",
                    u8"放棄上課，回房間"
                };
                int pick = menuSelect(opt, u8"傳達室的學姊問：『你沒帶學生證嗎？』\n");

                if (pick == 0) {
                    scene = (roommate >= 1) ? Scene::Classroom2 : Scene::Classroom3;
                } else if (pick == 1) {
                    scene = Scene::Dorm1;
                } else {
                    scene = Scene::EndingBad1;
                }
            } else {
                typePrint(u8"你掏出學生證，隨著嗶聲響起，電動門順利打開。\n");
                typePrint(u8"你和室友一起走去教室。\n");
                waitAnyKey();
                scene = Scene::Classroom1;
                
            }
        }

        else if (scene == Scene::Classroom1) {
            rlutil::locate(4, 0);
            rlutil::setColor(rlutil::YELLOW);
            cout << u8"教室\n\n";
            rlutil::setColor(rlutil::GREY);

            rlutil::locate(4, 4);
            typePrint(u8"你氣定神閒地推開教室門，和室友一同入座。\n");
            typePrint(u8"過了幾分鐘，老師走進教室打開投影幕：『現在開始掃QR code點名。』\n");
            typePrint(u8"你順利點到了名，為自己的大學生活創造了美好的開始。\n");
            study += 1;
            sign += 1;
            cout << u8"\n（簽到 = " << sign << u8"）\n";
            cout << u8"\n（知識 = " << study << u8"）\n";
            waitAnyKey();
            scene = Scene::Test;
        }

        else if (scene == Scene::Classroom2) {
            rlutil::locate(4, 0);
            rlutil::setColor(rlutil::YELLOW);
            cout << u8"教室\n\n";
            rlutil::setColor(rlutil::GREY);

            rlutil::locate(4, 4);
            typePrint(u8"你匆匆忙忙地推開教室門，室友連忙招手示意你過去他身旁的位置。\n");
            typePrint(u8"『點名已經結束了，』老師說，『下次請早。』\n");
            typePrint(u8"正當你沮喪之時，室友悄悄對你說：『放心，我幫你點好名了。』\n");
            typePrint(u8"你驚喜萬分，對室友再三感謝，晚上請他喝了一杯飲料。\n");
            sign += 1;
            rlutil::locate(4, 6);
            cout << u8"\n（簽到 = " << sign << u8"）\n";
            cout << u8"\n（知識 = " << study << u8"）\n";
            waitAnyKey();
            scene = Scene::Test;
        }

        else if (scene == Scene::EndingBad1) {
            rlutil::locate(4, 2);
            rlutil::setColor(rlutil::RED);
            cout << u8"結局：翹課\n\n";
            rlutil::setColor(rlutil::GREY);

            rlutil::locate(4, 4);
            typePrint(u8"你躺到床上，打算再次入眠。\n");
            typePrint(u8"這樣真的可以嗎？\n");
            typePrint(u8"自從這次翹課，你開始懶得去上課，學期末時不出意外被當掉了。\n");
            waitAnyKey(u8"按任意鍵結束遊戲...");
            scene = Scene::Quit;
        }

        else if (scene == Scene::Dorm1) {
            rlutil::locate(4, 2);
            rlutil::setColor(rlutil::YELLOW);
            cout << u8"宿舍\n\n";
            rlutil::setColor(rlutil::GREY);

            rlutil::locate(4, 4);
            typePrint(u8"你剛踏出一步，學姊立刻語氣不善地叫住你：『你有刷學生證嗎？』\n");
            typePrint(u8"你被學姊叫回去扣了十點，不只沒有準時到教室，下學年還不能抽宿舍。\n");
            study -= 1;
            cout << u8"\n（簽到 = " << sign << u8"）\n";
            cout << u8"\n（知識 = " << study << u8"）\n";
            waitAnyKey();
            scene = Scene::Test;
        }

        else if (scene == Scene::Classroom3) {
            rlutil::locate(4, 0);
            rlutil::setColor(rlutil::YELLOW);
            cout << u8"教室\n\n";
            rlutil::setColor(rlutil::GREY);

            rlutil::locate(4, 4);
            typePrint(u8"你匆匆忙忙地推開教室門，室友避開你的目光。\n");
            typePrint(u8"『點名已經結束了，』老師說，『下次請早。』\n");
            typePrint(u8"你尷尬地坐回自己的位置，心想下次一定要記得帶學生證。\n");
            cout << u8"\n（簽到 = " << sign << u8"）\n";
            cout << u8"\n（知識 = " << study << u8"）\n";
            waitAnyKey();
            scene = Scene::Test;
        }

        else if (scene == Scene::Test) {
            rlutil::locate(4, 0);
            rlutil::setColor(rlutil::YELLOW);
            cout << u8"教室\n\n";
            rlutil::setColor(rlutil::GREY);

            rlutil::locate(4, 4);
            typePrint(u8"今天是期末考，你坐在桌前緊張地等待考試開始。\n");
            typePrint(u8"你希望可以好好發揮自己上課學到的東西。\n");
            typePrint(u8"考完試後，你感到輕鬆許多。\n");
            typePrint(u8"而下周出的成績卻讓你大吃一驚──\n");
            waitAnyKey();
            if (study >= 1 && sign >= 1 && roommate >= 1) {
                scene = Scene::TrueEnding;
            } 
            else if (study >= 1 && sign >= 1) {
                scene = Scene::EndingGood;
            } 
            else if (sign >= 1) {
                scene = Scene::EndingNormal1;
            } 
            else if (study == 0 && sign == 0) {
                scene = Scene::EndingBad2;
            } 
            else {
                scene = Scene::EndingBad3;
            }
        }

        else if (scene == Scene::EndingGood) {
            rlutil::locate(4, 2);
            rlutil::setColor(rlutil::GREEN);
            cout << u8"結局：及格\n\n";
            rlutil::setColor(rlutil::GREY);

            rlutil::locate(4, 4);
            typePrint(u8"你利用上課學到的知識順利通過了期末考，簽到分數也讓你高空飛過，為自己的大學生活創造了美好的開始。\n");
            waitAnyKey(u8"按任意鍵結束遊戲...");
            scene = Scene::Quit;
        }

        else if (scene == Scene::EndingNormal1) {
            rlutil::locate(4, 2);
            rlutil::setColor(rlutil::YELLOW);
            cout << u8"結局：勉強及格\n\n";
            rlutil::setColor(rlutil::GREY);

            rlutil::locate(4, 4);
            typePrint(u8"你沒有通過期末考，但是看在你有都有來上課的份上，老師還是通融讓你以60的分數低空飛過。\n");
            waitAnyKey(u8"按任意鍵結束遊戲...");
            scene = Scene::Quit;
        }

        else if (scene == Scene::EndingBad2) {
            rlutil::locate(4, 2);
            rlutil::setColor(rlutil::RED);
            cout << u8"結局：不及格\n\n";
            rlutil::setColor(rlutil::GREY);

            rlutil::locate(4, 4);
            typePrint(u8"你沒有通過期末考，而平常總是遲到也讓你錯過很多簽到分數。\n");
            typePrint(u8"於是你只好寫信給教授求情，完成教授指定的三千字心得以求低分飛過。\n");
            waitAnyKey(u8"按任意鍵結束遊戲...");
            scene = Scene::Quit;
        }

        else if (scene == Scene::EndingBad3) {
            rlutil::locate(4, 2);
            rlutil::setColor(rlutil::RED);
            cout << u8"結局：重修就好\n\n";
            rlutil::setColor(rlutil::GREY);

            rlutil::locate(4, 4);
            typePrint(u8"你沒有通過期末考，而平常總是遲到也讓你錯過很多簽到分數。\n");
            typePrint(u8"於是你只好寫信給教授求情，但教授說期末考的分數太低，他也愛莫能助。\n");
            typePrint(u8"最後你只好重修這門課程。\n");
            waitAnyKey(u8"按任意鍵結束遊戲...");
            scene = Scene::Quit;
        }        

        else if (scene == Scene::TrueEnding) {
            rlutil::locate(4, 2);
            rlutil::setColor(rlutil::GREEN);
            cout << u8"結局：優異\n\n";
            rlutil::setColor(rlutil::GREY);

            rlutil::locate(4, 4);
            typePrint(u8"在準時上課還有和室友互相討論學習下，你不僅以優異的成績通過了期末考，而且還和室友建立了深厚的友誼。\n");
            typePrint(u8"除了學習，下課時間你也會和室友一起打遊戲、聊天。\n");
            typePrint(u8"你想，這就是你夢寐以求的大學生活。\n");
            waitAnyKey(u8"按任意鍵結束遊戲...");
            scene = Scene::Quit;
        }
    }

    rlutil::cls();
    rlutil::setColor(rlutil::GREY);
    cout << u8"遊戲結束。\n";
    return 0;
}
