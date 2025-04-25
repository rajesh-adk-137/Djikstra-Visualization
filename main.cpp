#include<SFML\Graphics.hpp>
#include <SFML/Window.hpp>
#include<SFML/Audio.hpp>
using namespace std;
using namespace sf;
#include<iostream>
#include <ctime>
#include<queue>
#include<vector>
#include<float.h>
#include<cstdlib>
#include <thread>
#include<chrono>

//For priority queue
typedef pair<vector<pair<int, int>>, int> q_pair;

//Comparison to make a min heap
struct myComp {
    constexpr bool operator()(q_pair const& a, q_pair const& b)
        const noexcept {
        return a.second > b.second;
    }
};




const int cell_size = 20;
const int w_cnt = 40;//width
const int h_cnt = 40;//height
int mouse_x = -1;//so no region is highlighted intitially
int mouse_y = -1;//so no region is highlighted intitially
int mouse_x_previous_value = -1;
int mouse_y_previous_value = -1;
int mouse_x_previous_value_2 = -1;//this is for hovering on ui elements
int mouse_y_previous_value_2 = -1;
int mouse_x_previous_value_3 = -1;//this is for drawing your own obstracle
int mouse_y_previous_value_3 = -1;

int mouse_x1 = -1;//for ui elements thingy
int mouse_y1 = -1;

int run = 0;//run checks if a randomized grid is already created or not..if it isn't then a new grid is created and run is set to 1.
int MAT[w_cnt][h_cnt];
int weight[w_cnt][h_cnt];
bool own_obstracle = false;  //this is set true when user wants to draw their own obstracle, if they press d 
int x_1 = -1, y_1 = -1, x_2 = -1, y_2 = -1;//our source and destination, initializing to -1 so there won't be weird bugs i guess!
int selected_points_in_grid = 0;
bool apply_dijkstra = false;
int selected_ui_element = -1;
int selected_ui_element2 = -1;//THIS is to highlight the ui element till it is pressed

Font font; //as the name suggest, it is font


bool exit_program = false;
bool find_path = false;
bool select_points = false;
bool own_grid = false;
bool new_grid = true;
bool isRightMousePressed = false;
bool select_point_activator = false;

bool repeated_click_on_find_path = false;

//New Variables declared
vector<pair<int, int>> adjacent[w_cnt][h_cnt]; //vector<pair<...>> used to contain the values of i and j i.e. pos of adjacent nodes
void displayAdjacency(int, int);//Declaration only, only to check

bool visited[w_cnt][h_cnt] = { 0 };//to check if the node is visited or not
bool dijk_done = false;//Only for final distance
bool show_path = false;//to show the path or not
bool dynamic_weight_on = false;

//priority queue for minheap
priority_queue<q_pair, vector<q_pair>, myComp> q;
vector<pair<int, int>> shortest_path;//to store the path of shortest distance

bool avoid_point_seletion_feature_when_dijksta_is_running = false;//to ignore point selected when the dijkstra is running

RectangleShape final_path(Vector2f(cell_size - 6, cell_size - 6));//Final Shortest Path

RenderWindow window(VideoMode((w_cnt + 20)* cell_size, h_cnt* cell_size), "Dijkstra", sf::Style::Close);
RectangleShape grid(Vector2f(cell_size - 1, cell_size - 1));
RectangleShape grid_highlighted1(Vector2f(cell_size - 0.5, cell_size - 1));
RectangleShape grid_highlighted2(Vector2f(cell_size - 0.5, cell_size - 1));
RectangleShape obstracle(Vector2f(cell_size - 1, cell_size - 1));
RectangleShape user_obstracle(Vector2f(cell_size - 1, cell_size - 1));
RectangleShape preview_border(sf::Vector2f(20 * cell_size, h_cnt* cell_size));
RectangleShape animation_block(sf::Vector2f(cell_size - 6, cell_size - 6));

RectangleShape weight2(Vector2f(cell_size - 0.5, cell_size - 1));
RectangleShape No_Path(Vector2f(50, 200));

sf::CircleShape circle(12);


bool Path_Error = false;

sf::Texture texture[10];
sf::RectangleShape options[10];

sf::RectangleShape options2[10];

Text text2[10];
Text text[10];


sf::Texture NoPath;

int** randomMatrix()//to randomly create the grid
{
    int** matrix = new int* [w_cnt];
    srand(time(NULL));
    for (int i = 0; i < w_cnt; i++) {
        matrix[i] = new int[w_cnt];
        for (int j = 0; j < h_cnt; j++) {
            int random = rand() % 10; // generate random number between 0 and 9
            if (random < 2) {
                matrix[i][j] = 1; // set element to 1 with probability of 0.3
            }
            else {
                matrix[i][j] = 0; // set element to 0 otherwise
            }
        }
    }
    return matrix;
}

Sound sfx_sound[5];
SoundBuffer buffer_sound[5];


bool play_path_found_song_once_everytime_path_is_found = false;

void initializers()
{
    for (int i = 0; i < w_cnt; i++)
    {
        for (int j = 0; j < h_cnt; j++)
        {
            weight[i][j] = 1;//intially assigning all weight to 1
        }
    }


    if (!font.loadFromFile("Lato-Regular.ttf"))
    {
        throw("Could not load font!");
    }

    buffer_sound[0].loadFromFile("button_click.ogg");
    sfx_sound[0].setBuffer(buffer_sound[0]);

    buffer_sound[1].loadFromFile("path_find.ogg");
    sfx_sound[1].setBuffer(buffer_sound[1]);


    buffer_sound[2].loadFromFile("place_points.ogg");
    sfx_sound[2].setBuffer(buffer_sound[2]);


    buffer_sound[3].loadFromFile("block.ogg");
    sfx_sound[3].setBuffer(buffer_sound[3]);


    buffer_sound[4].loadFromFile("runny.ogg");
    sfx_sound[4].setBuffer(buffer_sound[4]);


    texture[0].loadFromFile("side_image.jpg");
    texture[1].loadFromFile("wall.png");
    texture[2].loadFromFile("path.png");
    texture[3].loadFromFile("monkey.png");
    texture[4].loadFromFile("house.jpg");
    texture[5].loadFromFile("d.png");


    circle.setTexture(&texture[5]);
    circle.setPosition((w_cnt + 14) * cell_size - 2, 5 * cell_size + 17);


    NoPath.loadFromFile("nopath.png");

    text[0].setString("New Grid");
    text[0].setPosition((w_cnt + 7) * cell_size - 5, 5 * 1 * cell_size + 12);

    text[1].setString("Create Your Own");
    text[1].setPosition((w_cnt + 7) * cell_size - 53, 5 * 2 * cell_size + 12);

    text[2].setString("Select Points");
    text[2].setPosition((w_cnt + 7) * cell_size - 20, 5 * 3 * cell_size + 12);

    text[3].setString("Find Path");
    text[3].setPosition((w_cnt + 7) * cell_size - 5, 5 * 4 * cell_size + 12);

    text[4].setString("Exit");
    text[4].setPosition((w_cnt + 7) * cell_size + 32, 5 * 5 * cell_size + 12);


    text2[0].setString("New Grid");
    text2[0].setPosition((w_cnt + 7) * cell_size - 5, 5 * 1 * cell_size + 12);

    text2[1].setString("Create Your Own");
    text2[1].setPosition((w_cnt + 7) * cell_size - 59, 5 * 2 * cell_size + 12);

    text2[2].setString("Select Points");
    text2[2].setPosition((w_cnt + 7) * cell_size - 20, 5 * 3 * cell_size + 12);

    text2[3].setString("Find Path");
    text2[3].setPosition((w_cnt + 7) * cell_size - 5, 5 * 4 * cell_size + 12);

    text2[4].setString("Exit");
    text2[4].setPosition((w_cnt + 7) * cell_size + 32, 5 * 5 * cell_size + 12);



    for (int i = 1; i <= 5; i++)
    {

        options[i - 1].setSize(Vector2f(12 * cell_size, 3 * cell_size));
        options[i - 1].setOutlineThickness(1);
        options[i - 1].setOutlineColor(sf::Color(0, 220, 220, 150));
        options[i - 1].setPosition((w_cnt + 4) * cell_size, 5 * (i)*cell_size);
        options[i - 1].setFillColor(sf::Color(0, 120, 178, 200));

        text[i - 1].setFont(font);
        text[i - 1].setCharacterSize(27);
        text[i - 1].setStyle(Text::Bold);
        text[i - 1].setFillColor(sf::Color(255, 255, 255, 255));




        options2[i - 1].setSize(Vector2f(12 * cell_size + 10, 3 * cell_size + 10));
        options2[i - 1].setOutlineThickness(1);
        options2[i - 1].setOutlineColor(sf::Color(0, 220, 220, 150));
        options2[i - 1].setPosition((w_cnt + 4) * cell_size - 5, 5 * (i)*cell_size - 5);
        options2[i - 1].setFillColor(sf::Color(0, 100, 220, 255));

        text2[i - 1].setFont(font);
        text2[i - 1].setCharacterSize(29);
        text2[i - 1].setStyle(Text::Bold);
        text2[i - 1].setFillColor(sf::Color(255, 255, 0, 255));



    }
}

void draw_my_dij()
{
    if (show_path)
    {
        for (auto& p : shortest_path) {
            if ((x_1 != p.first || y_1 != p.second) && (x_2 != p.first || y_2 != p.second)) {
                // final_path.setFillColor(sf::Color(25, 130, 0, 255));
                final_path.setPosition(Vector2f((p.first) * cell_size + 3, (p.second) * cell_size + 3));
                final_path.setTexture(&texture[2]);
                window.draw(final_path);
            }
        }
        if (play_path_found_song_once_everytime_path_is_found == true)
        {
            sfx_sound[1].play();
            play_path_found_song_once_everytime_path_is_found = false;
        }

    }
    else
    {
        shortest_path = { {-1,-1} };


    }
}

void draw_options()
{

    for (int i = 1; i <= 5; i++)
    {
        if (i != selected_ui_element2)
        {

            window.draw(options[i - 1]);
            window.draw(text[i - 1]);

        }
    }



    if (selected_ui_element2 != -1 and selected_ui_element2 != 3)
    {


        window.draw(options2[selected_ui_element2 - 1]);
        window.draw(text2[selected_ui_element2 - 1]);

    }


    else if (selected_ui_element2 == 3 and show_path == false)//this one is to continuously activate point selector once it is clicked
    {


        window.draw(options2[3 - 1]);
        window.draw(text2[3 - 1]);
    }



    if (show_path == true)
    {


        window.draw(options2[4 - 1]);
        window.draw(text2[4 - 1]);
    }


    if (dynamic_weight_on == true)
    {
        window.draw(circle);
    }


}

void UI_elements_select()
{
    //cout << isRightMousePressed;

    if (mouse_x1 >= (w_cnt + 4) * cell_size and mouse_x1 <= (w_cnt + 16) * cell_size and mouse_y1 >= 5 * 1 * cell_size and mouse_y1 <= (3 + 5 * 5) * cell_size)
    {

        for (int i = 1; i < 6; i++)
        {
            if (mouse_y1 >= 5 * (i)*cell_size and mouse_y1 <= (3 + 5 * i) * cell_size)
            {
                selected_ui_element = i;
                break;
            }

        }
        if (isRightMousePressed)
        {

            if (selected_ui_element == 1)
            {

                new_grid = true;

            }
            else if (selected_ui_element == 2)
            {

                own_grid = true;

            }
            else if (selected_ui_element == 3)
            {

                select_points = true;

            }
            else if (selected_ui_element == 4)
            {
                if (apply_dijkstra == true or show_path == true)
                {
                    repeated_click_on_find_path = true;
                }
                else if (show_path == false)
                {
                    repeated_click_on_find_path = false;
                    sfx_sound[0].play();
                }
                find_path = true;



            }
            else if (selected_ui_element == 5)
            {
                exit_program = true;

            }
        }




        if (isRightMousePressed == true)
        {

            selected_ui_element2 = selected_ui_element;
        }
        else if (isRightMousePressed == false)
        {
            if (selected_ui_element2 != 3)//to continuosly highlihht the poin selection button if it is pressed
                selected_ui_element2 = -1;
        }


        selected_ui_element = -1;
    }

}

void UI_elements_handler()
{
    if (new_grid == true)
    {
        sfx_sound[0].play();
        new_grid = false;
        own_obstracle = false;
        select_point_activator = false;
        apply_dijkstra = false;
        run = 0;

        Path_Error = false;

        show_path = false;
        for (int i = 0; i < w_cnt; i++)
            for (int j = 0; j < h_cnt; j++)
                visited[i][j] = false;

        mouse_x = -1;//so no grid is highlighted after changing mode
        mouse_y = -1;// so no grid is highlighted after changing mode
        x_1 = -1;// so no grid is highlighted after changing mode
        y_1 = -1;// so no grid is highlighted after changing mode
        y_2 = -1;// so no grid is highlighted after changing mode
        x_2 = -1;// so no grid is highlighted after changing mode
        selected_points_in_grid = 0;



    }
    else if (own_grid == true)
    {
        sfx_sound[0].play();
        own_grid = false;
        own_obstracle = true;
        select_point_activator = false;
        apply_dijkstra = false;

        Path_Error = false;
        show_path = false;
        for (int i = 0; i < w_cnt; i++)
            for (int j = 0; j < h_cnt; j++)
                visited[i][j] = false;

        for (int i = 0; i < w_cnt; i++)
        {
            for (int j = 0; j < h_cnt; j++)
            {
                MAT[i][j] = 0;//making the obstracle grid empty
                weight[i][j] = 1;//making it 1 by default


                if (dynamic_weight_on == true)//if dynamic weight is on only then randomized weight will be called
                {
                    int a = rand() % 2;
                    if (a == 0)
                        weight[i][j] = 2;
                }

            }
        }
        mouse_x = -1;//so no grid is highlighted after changing mode
        mouse_y = -1;// so no grid is highlighted after changing mode
        x_1 = -1;// so no grid is highlighted after changing mode
        y_1 = -1;// so no grid is highlighted after changing mode
        y_2 = -1;// so no grid is highlighted after changing mode
        x_2 = -1;// so no grid is highlighted after changing mode
        selected_points_in_grid = 0;
    }
    else if (select_points == true)
    {
        select_points = false;
        sfx_sound[0].play();
        avoid_point_seletion_feature_when_dijksta_is_running = false;
        mouse_x = mouse_y = -1;//to clear any previous points selected

        select_point_activator = true;
        apply_dijkstra = false;

        Path_Error = false;
        show_path = false;
        for (int i = 0; i < w_cnt; i++)
            for (int j = 0; j < h_cnt; j++)
                visited[i][j] = false;
    }
    else if (find_path == true)
    {
        //select_point_activator = false;

        find_path = false;
        if (selected_points_in_grid == 2)
        {
            dijk_done = false;
            apply_dijkstra = true;

            show_path = true;
            avoid_point_seletion_feature_when_dijksta_is_running = true;

        }
        else
        {
            cout << "select atleast 2 points";
            apply_dijkstra = false;
            avoid_point_seletion_feature_when_dijksta_is_running = false;

        }
    }
}


void draw_grid()
{
    for (int a = 0; a < w_cnt; a++)
        for (int b = 0; b < h_cnt; b++)
        {
            if (weight[a][b] != 2 or dynamic_weight_on == false)
            {

                grid.setPosition(cell_size * (a), cell_size * (b));
                grid.setFillColor(sf::Color(0, 150, 150, 150));
                //grid.setFillColor(sf::Color(255, 255, 255, 255));
                window.draw(grid);
            }

            if (weight[a][b] == 2 and dynamic_weight_on == true)
            {
                //int shade = 150;
                weight2.setFillColor(sf::Color(0, 150, 150, 100));
                weight2.setPosition(Vector2f((a)*cell_size, (b)*cell_size));
                window.draw(weight2);
            }

        }

}




void draw_obstracle()
{

    if (run == 0)//run=0 means run is true, i.e it is a first loading, so we need to initialize the position of the obstracle
    {
        int** matrix = randomMatrix();
        for (int i = 0; i < w_cnt; i++)
        {
            for (int j = 0; j < h_cnt; j++)
            {
                MAT[i][j] = matrix[i][j]; //MAT[i][j] is 1 in those places where obstacle exists
            }
        }
        run = 1;

        for (int i = 0; i < w_cnt; i++)
        {
            for (int j = 0; j < h_cnt; j++)
            {
                weight[i][j] = 1;////intially assigning all weight to 1
                if (MAT[i][j])
                {
                    weight[i][j] = -1;
                }



                if (dynamic_weight_on == true)//if dynamic weight is on only then randomized weight will be called
                {
                    int a = rand() % 2;
                    if (a == 0 and weight[i][j] != -1)
                        weight[i][j] = 2;
                }
            }
        }
        //cout << "a";
    }
    // cout << "a";
    for (int i = 0; i < w_cnt; i++)
    {
        for (int j = 0; j < h_cnt; j++)
        {
            if (MAT[i][j] == 1)
            {
                //weight[i][j] = -1;
                //obstracle.setFillColor(sf::Color(0, 26, 0, 255));
                obstracle.setTexture(&texture[1]);
                obstracle.setPosition(Vector2f((i)*cell_size, (j)*cell_size));
                window.draw(obstracle);
            }
            /*  else if (weight[i][j] == 2)
              {
                  //int shade = 150;
                  weight2.setFillColor(sf::Color(150, 150, 0, 100));
                  weight2.setPosition(Vector2f((i)*cell_size, (j)*cell_size));
                  window.draw(weight2);
              }*/
        }
    }
}//debugged recently


void draw_user_obstracle()
{
    if (select_point_activator == false)
    {
        if (mouse_x != mouse_x_previous_value_3 or mouse_y != mouse_y_previous_value_3)
        {
            if (mouse_x >= 0 and mouse_y >= 0 and mouse_x <= cell_size * w_cnt and mouse_y <= cell_size * h_cnt)
            {
                sfx_sound[3].play();

                int i = -1, j = -1;


                for (i = 0; i < w_cnt; i++)

                {

                    if (mouse_x > i * cell_size and mouse_x <= (i + 1) * cell_size)
                        break;
                }


                for (j = 0; j < h_cnt; j++)
                {
                    if (mouse_y > j * cell_size and mouse_y <= (j + 1) * cell_size)
                        break;
                }

                MAT[i][j] = 1;
                weight[i][j] = -1;


            }

            mouse_x_previous_value_3 = mouse_x;
            mouse_y_previous_value_3 = mouse_y;

        }
    }


    for (int i = 0; i < w_cnt; i++)
        for (int j = 0; j < h_cnt; j++)
        {

            if (MAT[i][j] == 1)
            {

                //user_obstracle.setFillColor(sf::Color(0, 26, 0, 255));
                user_obstracle.setTexture(&texture[1]);
                user_obstracle.setPosition(Vector2f((i)*cell_size, (j)*cell_size));
                window.draw(user_obstracle);
            }
        }


}//debugged recently

//Does not matter, only to check if adjacency list is correct
int temp_x = -1, temp_y = -1;
bool displayed1 = false;
bool displayed2 = false;

//this function does not matter, only here to check for adjacency list of the node when clicked
bool checkfunction(int x, int y) {
    if (temp_x != x and temp_y != y) {
        temp_x = x;
        temp_y = y;
        return false;
    }
    return true;
}

void draw_grid_highlighted()
{

    if (mouse_x != mouse_x_previous_value and mouse_y != mouse_y_previous_value and avoid_point_seletion_feature_when_dijksta_is_running == false)// and select_point_activator == true)
    {


        int i = 0, j = 0;

        for (i = 0; i < w_cnt; i++)

        {

            if (mouse_x > i * cell_size and mouse_x <= (i + 1) * cell_size)
                break;
        }


        for (j = 0; j < h_cnt; j++)
        {
            if (mouse_y > j * cell_size and mouse_y <= (j + 1) * cell_size)
                break;
        }

        if (MAT[i][j] == 0)
        {
            sfx_sound[2].play();

            if (selected_points_in_grid == 0)
            {
                x_1 = i;
                y_1 = j;

                selected_points_in_grid = 1;


            }
            else if (selected_points_in_grid == 1)
            {
                if (i == x_1 and j == y_1) //this is my ocd
                {
                    x_1 = y_1 = -1;
                    y_2 = x_2 = -1;
                    selected_points_in_grid = 0;
                }
                else
                {
                    x_2 = i;
                    y_2 = j;
                    selected_points_in_grid = 2;
                }


            }

            else if (selected_points_in_grid == 2)
            {
                if (i == x_1 and j == y_1) //this is my ocd
                {
                    x_1 = x_2;
                    y_1 = y_2;
                    x_2 = y_2 = -1;
                    selected_points_in_grid = 1;
                }
                else if (i == x_2 and j == y_2)//this is my ocd
                {

                    x_2 = y_2 = -1;
                    selected_points_in_grid = 1;
                }

                else
                {

                    x_1 = x_2;
                    y_1 = y_2;
                    y_2 = j;
                    x_2 = i;
                }

            }


        }
        mouse_y_previous_value = mouse_y;
        mouse_x_previous_value = mouse_x;

    }

    if (x_1 != -1 and y_1 != -1)
    {
        if (!displayed1) {
            displayAdjacency(x_1, y_1);
            displayed1 = true;
        }
        //grid_highlighted.setFillColor(sf::Color(255, 130, 0, 255));
        grid_highlighted1.setTexture(&texture[3]);
        grid_highlighted1.setPosition(Vector2f((x_1)*cell_size, (y_1)*cell_size));
        window.draw(grid_highlighted1);
    }
    if (x_2 != -1 and y_2 != -1)
    {
        //to check for adjacency list. Here and not above because after first click, in every other click x2 and y2 are the selected points while prev pair shifts to x1 and y1
        if (!checkfunction(x_2, y_2)) {
            displayed2 = false;
        }
        if (!displayed2) {
            displayAdjacency(int(x_2), int(y_2));
            displayed2 = true;
        }
        //grid_highlighted.setFillColor(sf::Color(255, 255, 0, 255));
        grid_highlighted2.setTexture(&texture[4]);
        grid_highlighted2.setPosition(Vector2f((x_2)*cell_size, (y_2)*cell_size));
        window.draw(grid_highlighted2);
    }
}

void draw_preview_border()
{
    //preview_border.setFillColor(sf::Color(1, 100, 200, 200));
    preview_border.setTexture(&texture[0]);
    preview_border.setOutlineThickness(0.5);
    preview_border.setPosition(w_cnt * cell_size, 0);
    window.draw(preview_border);

}



void display(int x, int y)
{

    // std::this_thread::sleep_for(std::chrono::milliseconds(20));
    if (x != x_1 || y != y_1) {

        animation_block.setFillColor(sf::Color(25, 130, 0, 255));
        animation_block.setPosition(Vector2f(x * cell_size + 3, y * cell_size + 3));
     
        
        for (int i = 0; i < 5; i++)
        {
            
            window.draw(animation_block);
            window.display();
            draw_options();
            
        }
       

    }
}

void dijkstra(int x_1, int  y_1, int  x_2, int y_2)
{
    vector<pair<int, int>> src = { { x_1, y_1 } };
    q.emplace(src, 0);//pushes the starting node and its distance i.e. 0 to the priority queue
    int _distance = 0;
    int shortest_distance = 0;
    int flag = 0;
    play_path_found_song_once_everytime_path_is_found = false;


    while (!q.empty())
    {
        auto node_distance = q.top();//node_distance meaning nodesTraversed and the distance from the source
        q.pop();
        _distance = node_distance.second; //second data in the container stores distance info
        int i = node_distance.first.back().first;//last pair of the vector i.e. Grid for BFS
        int j = node_distance.first.back().second;
        // cout << endl << "i, j " << i << "\t" << j << " Distance : " << _distance << endl;//only here to check which node is being traversed. No need

        if (visited[i][j])
        {

            continue;
        }

        display(i, j); //this function displays the progression of the algorithm to the destination point
        std::this_thread::sleep_for(std::chrono::milliseconds(2));

        visited[i][j] = true;//once visited then true
        if (i == x_2 && j == y_2) {
            flag = 1;
            shortest_distance = _distance;
            shortest_path = node_distance.first;//final path, since now we have the destination node
            break;//if destination node then break
        }
        for (auto& p : adjacent[i][j]) {//pushes all the adjacent nodes to the queue
            if (!visited[p.first][p.second]) {
                vector<pair<int, int>> NodeQueue = node_distance.first;//takes the vector/container so that it then can take the adjacent node and thus be pushed to the queue
                NodeQueue.emplace_back(p.first, p.second);//pushes the adjacent node to the back of the vector
                int new_distance = _distance + weight[p.first][p.second];
                q.emplace(NodeQueue, new_distance);//pushes the adjacent nodes and their corresponding distances form source
            }
        }
    }
    if (q.empty() && flag == 0)
    {


        dijk_done = true;
        apply_dijkstra = false;
        if (repeated_click_on_find_path == false)
        {
            cout << " No Path exists " << endl;
            Path_Error = true;
        }
        play_path_found_song_once_everytime_path_is_found = false;
        return;

    }

    if (!dijk_done) {
        cout << "Shortest Distance : " << shortest_distance << endl;//number of transitions required i.e. steps
        cout << "Shortest Path" << endl;
        play_path_found_song_once_everytime_path_is_found = true;
        for (auto& p : shortest_path) {
            cout << "(" << p.first << "," << p.second << ")\t";
        }
        while (!q.empty()) {//To check and also empty the queue
            // cout << q.top().second << "\t";
            q.pop();
        }
        dijk_done = true;
        apply_dijkstra = false;
    }
}

//to create adjacency list excluding diagonal nodes
void adjacency_list()
{
    for (int i = 0; i < w_cnt; i++) {
        for (int j = 0; j < h_cnt; j++) {
            adjacent[i][j].clear();
            for (int k = max(i - 1, 0); k <= min(i + 1, w_cnt - 1); k++) { //makes sure value of k exists within the columns defined. initialization makes sure the value doesn't go below 0 and starts from either i-1 or 0 while termination condition makes sure value doesn't go past i+1 or w_cnt-1 whichever minimum
                if (k == i - 1 || k == i + 1) {// remove this if diagonal nodes are to be included
                    if (weight[k][j] != -1) {
                        adjacent[i][j].emplace_back(make_pair(k, j));
                        continue;
                    }
                    if (weight[k][j] == -1)
                        continue;
                }

                for (int l = max(j - 1, 0); l <= min(j + 1, h_cnt - 1); l++) {//same as above, but in rows defined

                    if ((k != i || l != j) && weight[k][l] != -1) { // excludes the node itself and the obstackle
                        adjacent[i][j].emplace_back(make_pair(k, l));//adds to the container/vector from behind
                    }
                }
            }
        }
    }
}


//only to check the adjacent nodes, doesn't matter much for the program
void displayAdjacency(int i, int j)
{
    //for (auto& p : adjacent[i][j])
       // cout << "(" << p.first << "," << p.second << ") ";
   // cout << endl;
}




int main()
{
    bool isMousePressed = false;
    bool d_pressed_checker = false;

    initializers();


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {





            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D)
            {

                if (d_pressed_checker == false)
                {

                    dynamic_weight_on = true;
                    d_pressed_checker = true;
                }
                else  if (d_pressed_checker == true)
                {

                    dynamic_weight_on = false;
                    d_pressed_checker = false;
                }
                if (own_obstracle == true)
                {
                    own_grid = true;

                }
                else if (own_obstracle == false)
                {
                    new_grid = true;

                }
                selected_ui_element2 = -1;//to unhighlight any buttons if previosly pressed


            }



            if (event.type == sf::Event::Closed or exit_program == true)
            {

                window.close();
                exit(0);


            }


            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {

                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                mouse_x1 = mousePosition.x;
                mouse_y1 = mousePosition.y;
                isRightMousePressed = true;
                UI_elements_select();
                UI_elements_handler();
                draw_options();



            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                mouse_x1 = mousePosition.x;
                mouse_y1 = mousePosition.y;
                isRightMousePressed = false;
                UI_elements_select();
                UI_elements_handler();
                draw_options();
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left and own_obstracle == true)

            {
                isMousePressed = true;
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left and own_obstracle == true)
            {
                isMousePressed = false;
            }
            if (isMousePressed)
            {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                mouse_x = mousePosition.x;
                mouse_y = mousePosition.y;
            }





            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left and own_obstracle == false)
            {

                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

                mouse_x = mousePosition.x;
                mouse_y = mousePosition.y;


            }




        }
        
            window.clear();
        draw_preview_border();

        draw_grid();
        UI_elements_handler();
        draw_options();



        if (own_obstracle == true)
        {
            draw_user_obstracle();
            if (select_point_activator == true)
            {
                draw_grid_highlighted();
            }

        }
        else if (own_obstracle == false and select_point_activator == false)
        {
            draw_obstracle();

        }
        else if (own_obstracle == false and select_point_activator == true)
        {
            draw_obstracle();
            draw_grid_highlighted();
        }
        adjacency_list();



        if (apply_dijkstra == true)
        {
            
            dijkstra(x_1, y_1, x_2, y_2);

        }

        draw_my_dij();

        if (Path_Error)
        {
            No_Path.setPosition(window.getSize().x * 0.1, window.getSize().y * 0.35);
            No_Path.setTexture(&NoPath);
            window.draw(No_Path);

        }
        if (apply_dijkstra == false)
            window.display();

    }


}
