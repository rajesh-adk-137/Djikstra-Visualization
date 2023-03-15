#include<SFML/Graphics.hpp>
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
Text text[5];// text

bool exit_program = false;
bool find_path = false;
bool select_points = false;
bool own_grid = false;
bool new_grid = true;
bool isRightMousePressed = false;
bool select_point_activator = false;

//New Variables declared
vector<pair<int, int>> adjacent[w_cnt][h_cnt]; //vector<pair<...>> used to contain the values of i and j i.e. pos of adjacent nodes
void displayAdjacency(int, int);//Declaration only, only to check

bool visited[w_cnt][h_cnt] = { 0 };//to check if the node is visited or not
bool dijk_done = false;//Only for final distance
bool show_path = false;//to show the path or not

//priority queue for minheap
priority_queue<q_pair, vector<q_pair>, myComp> q;
vector<pair<int, int>> shortest_path;//to store the path of shortest distance

RectangleShape final_path(Vector2f(cell_size - 0.5, cell_size - 0.5));//Final Shortest Path

RenderWindow window(VideoMode((w_cnt + 20)* cell_size, h_cnt* cell_size), "Dijkstra", sf::Style::Close);
RectangleShape grid(Vector2f(cell_size - 1, cell_size - 1));
RectangleShape grid_highlighted(Vector2f(cell_size - 0.5, cell_size - 1));
RectangleShape obstracle(Vector2f(cell_size - 0.5, cell_size - 0.5));
RectangleShape user_obstracle(Vector2f(cell_size - 0.5, cell_size - 0.5));
RectangleShape preview_border(sf::Vector2f(20 * cell_size, h_cnt* cell_size));
//block for visited nodes
RectangleShape animation_block(sf::Vector2f(cell_size - 0.5, cell_size - 1));

sf::RectangleShape options[5];
//(sf::Vector2f(12 * cell_size, 3 * cell_size));

int** randomMatrix()//to randomly create the grid
{
    int** matrix = new int* [w_cnt];
    srand(time(NULL));
    for (int i = 0; i < w_cnt; i++) {
        matrix[i] = new int[w_cnt];
        for (int j = 0; j < h_cnt; j++) {
            int random = rand() % 10; // generate random number between 0 and 9
            if (random < 3) {
                matrix[i][j] = 1; // set element to 1 with probability of 0.3
            }
            else {
                matrix[i][j] = 0; // set element to 0 otherwise
            }
        }
    }
    return matrix;
}

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


    text[0].setString("New Grid");
    text[0].setPosition((w_cnt + 7) * cell_size - 5, 5 * 1 * cell_size + 12);

    text[1].setString("Create your own");
    text[1].setPosition((w_cnt + 7) * cell_size - 49, 5 * 2 * cell_size + 12);

    text[2].setString("Select Points");
    text[2].setPosition((w_cnt + 7) * cell_size - 20, 5 * 3 * cell_size + 12);

    text[3].setString("Find Path");
    text[3].setPosition((w_cnt + 7) * cell_size - 5, 5 * 4 * cell_size + 12);

    text[4].setString("Exit");
    text[4].setPosition((w_cnt + 7) * cell_size + 32, 5 * 5 * cell_size + 12);

    for (int i = 1; i <= 5; i++)
    {
        options[i - 1].setSize(Vector2f(12 * cell_size, 3 * cell_size));
        options[i - 1].setOutlineThickness(1);
        options[i - 1].setOutlineColor(sf::Color(0, 0, 0, 150));
        options[i - 1].setPosition((w_cnt + 4) * cell_size, 5 * (i)*cell_size);
        options[i - 1].setFillColor(sf::Color(0, 255, 0, 150));

        text[i - 1].setFont(font);
        text[i - 1].setCharacterSize(27);
        text[i - 1].setStyle(Text::Bold);
        text[i - 1].setFillColor(sf::Color(255, 255, 255, 255));



    }
}

void draw_options()
{
    for (int i = 1; i <= 5; i++)
    {
        if (i != selected_ui_element)
        {
            options[i - 1].setPosition((w_cnt + 4) * cell_size, 5 * (i)*cell_size);
            options[i - 1].setSize(Vector2f(12 * cell_size, 3 * cell_size));
            options[i - 1].setFillColor(sf::Color(0, 255, 0, 150));


            text[i - 1].setCharacterSize(27);


            window.draw(options[i - 1]);
            window.draw(text[i - 1]);
        }
    }
    if (selected_ui_element2 != -1 and selected_ui_element2 != 3)
    {
        options[selected_ui_element2 - 1].setPosition((w_cnt + 4) * cell_size - 5, 5 * (selected_ui_element2)*cell_size - 5);
        options[selected_ui_element2 - 1].setSize(Vector2f(12 * cell_size + 10, 3 * cell_size + 10));
        options[selected_ui_element2 - 1].setFillColor(sf::Color(0, 179, 0, 255));


        text[selected_ui_element2 - 1].setCharacterSize(29);

        window.draw(options[selected_ui_element2 - 1]);
        window.draw(text[selected_ui_element2 - 1]);
    }
    if (selected_ui_element2 == 3 and apply_dijkstra == false)//this one is to continuously activate point selector once it is clicked
    {
        options[3 - 1].setPosition((w_cnt + 4) * cell_size - 5, 5 * (3) * cell_size - 5);
        options[3 - 1].setSize(Vector2f(12 * cell_size + 10, 3 * cell_size + 10));
        options[3 - 1].setFillColor(sf::Color(0, 179, 0, 255));


        text[3 - 1].setCharacterSize(29);

        window.draw(options[3 - 1]);
        window.draw(text[3 - 1]);
    }
    if (apply_dijkstra == true)
    {
        options[4 - 1].setPosition((w_cnt + 4) * cell_size - 5, 5 * (4) * cell_size - 5);
        options[4 - 1].setSize(Vector2f(12 * cell_size + 10, 3 * cell_size + 10));
        options[4 - 1].setFillColor(sf::Color(0, 179, 0, 255));


        text[4 - 1].setCharacterSize(29);

        window.draw(options[4 - 1]);
        window.draw(text[4 - 1]);
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
        new_grid = false;
        own_obstracle = false;
        select_point_activator = false;
        apply_dijkstra = false;
        run = 0;

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
        own_grid = false;
        own_obstracle = true;
        select_point_activator = false;
        apply_dijkstra = false;

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
        mouse_x = mouse_y = -1;//to clear any previous points selected
        select_points = false;
        select_point_activator = true;
        apply_dijkstra = false;

        show_path = false;
        for (int i = 0; i < w_cnt; i++)
            for (int j = 0; j < h_cnt; j++)
                visited[i][j] = false;
    }
    else if (find_path == true)
    {
        find_path = false;
        if (selected_points_in_grid == 2)
        {
            dijk_done = false;
            apply_dijkstra = true;

            show_path = true;
        }
        else
        {
            cout << "select atleast 2 points";
            apply_dijkstra = false;

        }
    }
}


void draw_grid()
{
    for (int a = 0; a < w_cnt; a++)
        for (int b = 0; b < h_cnt; b++)
        {

            grid.setPosition(cell_size * (a), cell_size * (b));
            grid.setFillColor(sf::Color(200, 255, 255, 255));
            window.draw(grid);

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
                else
                {
                    int a = rand() % 2;
                    if (a == 0)
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
            if (weight[i][j] == -1)
            {
                //weight[i][j] = -1;
                obstracle.setFillColor(sf::Color(0, 26, 0, 255));
                obstracle.setPosition(Vector2f((i)*cell_size, (j)*cell_size));
                window.draw(obstracle);
            }
            else if (weight[i][j] == 2)
            {
                int shade = 150;
                obstracle.setFillColor(sf::Color(shade, shade + 55, shade + 55, 255));
                obstracle.setPosition(Vector2f((i)*cell_size, (j)*cell_size));
                window.draw(obstracle);
            }
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

                user_obstracle.setFillColor(sf::Color(0, 26, 0, 255));
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

    if (mouse_x != mouse_x_previous_value and mouse_y != mouse_y_previous_value)

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
        grid_highlighted.setFillColor(sf::Color(255, 130, 0, 255));
        grid_highlighted.setPosition(Vector2f((x_1)*cell_size, (y_1)*cell_size));
        window.draw(grid_highlighted);
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
        grid_highlighted.setFillColor(sf::Color(255, 255, 0, 255));
        grid_highlighted.setPosition(Vector2f((x_2)*cell_size, (y_2)*cell_size));
        window.draw(grid_highlighted);
    }
}

void draw_preview_border()
{
    preview_border.setFillColor(sf::Color(1, 100, 200, 200));
    preview_border.setOutlineThickness(0.5);
    preview_border.setPosition(w_cnt * cell_size, 0);
    window.draw(preview_border);

}



void display(int x, int y){
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    animation_block.setFillColor(sf::Color(190, 50, 50, 255));
    animation_block.setPosition(Vector2f(x * cell_size, y * cell_size));
    for(int i=0; i<=24; i++){
        window.draw(animation_block);
        window.display();
    }

}



void dijkstra(int x_1, int  y_1, int  x_2, int y_2)
{

    SoundBuffer soundBuffer;
    if(!soundBuffer.loadFromFile("beep.wav")){
        exit;
    }

    Sound sound;
    sound.setBuffer(soundBuffer);
    //to do the algorithm thingy
    vector<pair<int, int>> src = { { x_1, y_1 } };
    q.emplace(src, 0);//pushes the starting node and its distance i.e. 0 to the priority queue
    int _distance = 0;
    int shortest_distance = 0;
    int flag = 0;
    while (!q.empty()) {
        auto node_distance = q.top();//node_distance meaning nodesTraversed and the distance from the source
        q.pop();
        _distance = node_distance.second; //second data in the container stores distance info
        int i = node_distance.first.back().first;//last pair of the vector i.e. Grid for BFS
        int j = node_distance.first.back().second;
        cout << endl << "i, j " << i << "\t" << j << " Distance : " << _distance << endl;//only here to check which node is being traversed. No need

        if (visited[i][j])
            continue;
        visited[i][j] = true;//once visited then true

        display(i, j); //this function displays the progression of the algorithm to the destination point

        sound.play();
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
    if (q.empty() && flag == 0) {
        cout << " No Path exists " << endl;
        dijk_done = true;
        apply_dijkstra = false;
        return;
    }

    if (!dijk_done) {
        cout << "Shortest Distance : " << shortest_distance << endl;//number of transitions required i.e. steps
        cout << "Shortest Path" << endl;
        for (auto& p : shortest_path) {
            cout << "(" << p.first << "," << p.second << ")\t";
        }
        while (!q.empty()) {//To check and also empty the queue
            cout << q.top().second << "\t";
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
            for (int k = max(i - 1, 0); k <= min(i + 1, w_cnt - 1); k++) { //makes sure value of k exits within the columns defined. initialization makes sure the value doesn't go below 0 and starts from either i-1 or 0 while termination condition makes sure value doesn't go past i+1 or w_cnt-1 whichever minimum
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
void displayAdjacency(int i, int j) {
    for (auto& p : adjacent[i][j])
        cout << "(" << p.first << "," << p.second << ") ";
    cout << endl;
}

int main()
{
    bool isMousePressed = false;
    initializers();


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
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


            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                mouse_x1 = mousePosition.x;
                mouse_y1 = mousePosition.y;
                isRightMousePressed = false;
                UI_elements_select();
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
        draw_grid();
        draw_preview_border();
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
        if (show_path) {
            for (auto& p : shortest_path) {
                if ((x_1 != p.first || y_1 != p.second) && (x_2 != p.first || y_2 != p.second)) {
                    final_path.setFillColor(sf::Color::Green);
                    final_path.setPosition(Vector2f((p.first) * cell_size, (p.second) * cell_size));
                    window.draw(final_path);
                }
            }
        }
        else
        {
            shortest_path = { {0,0} };
        }

        window.display();
    }


}
