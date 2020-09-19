#include <igl/read_triangle_mesh.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <sstream>
#include <igl/opengl/glfw/Viewer.h>


Eigen::MatrixXd V;
Eigen::MatrixXi F;
// The Viewer
igl::opengl::glfw::Viewer viewer;
// The (optional) viewer menu
igl::opengl::glfw::imgui::ImGuiMenu menu;

// These are to showcase the menu box (imgui) capacity. See below.
double doubleVariable = 0.1f; // Shared between two menus
bool boolVariable = true;
enum Orientation { Up=0, Down, Left, Right };
Orientation dir = Up;
int num_choices = 3;
std::vector<std::string> choices;
int idx_choice = 0;


// This function is called every time a keyboard button is pressed
bool key_down(igl::opengl::glfw::Viewer& viewer, unsigned char key, int modifier);

// This function draws the menu boxes
void draw_menu_boxes();

// Function to draw bounding box of geometry
void draw_bbox();

// Function to compute bounding box of geometry
void compute_bbox(const Eigen::MatrixXd &V,
                  Eigen::Vector3d &m,
                  Eigen::Vector3d &M,
                  Eigen::MatrixXd &V_box,
                  Eigen::MatrixXi &E_box);


bool key_down(igl::opengl::glfw::Viewer& viewer, unsigned char key, int modifier)
{
  if (key == '1')
  {
    std::cout<<"Hiding box"<<std::endl;
    viewer.data().clear_edges();
    viewer.data().clear_points();
    viewer.data().clear_labels();
  }
  else if (key == '2')
  {
    std::cout<<"Showing box"<<std::endl;
    viewer.data().clear_edges();
    draw_bbox();
  }
  
  return false;
}

void draw_menu_boxes()
{
  // Enable the ImGui plugin to show text labels and menu boxes.
  viewer.plugins.push_back(&menu);

  
  // Customize the menu
  // Add content to the default menu window
  menu.callback_draw_viewer_menu = [&]()
  {
    // Draw parent menu content
    menu.draw_viewer_menu();

    // Add new group
    if (ImGui::CollapsingHeader("New Group", ImGuiTreeNodeFlags_DefaultOpen))
    {
      // Expose variable directly ...
      ImGui::InputDouble("double", &doubleVariable, 0, 0, "%.4f");

      // ... or using a custom callback
      if (ImGui::Checkbox("bool", &boolVariable))
      {
        // do something
        std::cout << "boolVariable: " << std::boolalpha << boolVariable << std::endl;
      }

      // Expose an enumeration type
      ImGui::Combo("Direction", (int *)(&dir), "Up\0Down\0Left\0Right\0\0");

      // We can also use a std::vector<std::string> defined dynamically
      if (ImGui::InputInt("Num letters", &num_choices))
      {
        num_choices = std::max(1, std::min(26, num_choices));
      }
      if (num_choices != (int) choices.size())
      {
        choices.resize(num_choices);
        for (int i = 0; i < num_choices; ++i)
          choices[i] = std::string(1, 'A' + i);
        if (idx_choice >= num_choices)
          idx_choice = num_choices - 1;
      }
      ImGui::Combo("Letter", &idx_choice, choices);

      // Add a button
      if (ImGui::Button("Print Hello", ImVec2(-1,0)))
      {
        std::cout << "Hello\n";
      }
    }
  };

  // Draw additional windows
  menu.callback_draw_custom_window = [&]()
  {
    // Define next window position + size
    ImGui::SetNextWindowPos(ImVec2(180.f * menu.menu_scaling(), 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(200, 160), ImGuiCond_FirstUseEver);
    ImGui::Begin(
        "New Window", nullptr,
        ImGuiWindowFlags_NoSavedSettings
    );

    // Expose the same variable directly ...
    ImGui::PushItemWidth(-80);
    ImGui::DragScalar("double", ImGuiDataType_Double, &doubleVariable, 0.1, 0, 0, "%.4f");
    ImGui::PopItemWidth();

    static std::string str = "mesh";
    ImGui::InputText("Name", str);

    ImGui::End();
  };

}

// Function to draw bounding box of geometry
void draw_bbox()
{
  Eigen::MatrixXd V_box;
  Eigen::MatrixXi E_box;
  Eigen::Vector3d m, M;
  // Compute the bounding box
  compute_bbox(V, m, M, V_box, E_box);

  
  // Plot the corners of the bounding box as points
  Eigen::RowVector3d bb_color(121./255, 165./255, 237./255);
  viewer.data().add_points(V_box,bb_color);

  // Plot the edges of the bounding box
  for (unsigned i=0;i<E_box.rows(); ++i)
    viewer.data().add_edges
    (
      V_box.row(E_box(i,0)),
      V_box.row(E_box(i,1)),
     bb_color
    );

  // Plot labels with the coordinates of bounding box vertices
  std::stringstream l1;
  l1 << m(0) << ", " << m(1) << ", " << m(2);
  viewer.data().add_label(m+Eigen::Vector3d(-0.007, 0, 0),l1.str());
  std::stringstream l2;
  l2 << M(0) << ", " << M(1) << ", " << M(2);
  viewer.data().add_label(M+Eigen::Vector3d(0.007, 0, 0),l2.str());
  // activate label rendering
  viewer.data().show_custom_labels = true;

}


int main(int argc, char *argv[])
{

  // Load a mesh in OFF format
  if (argc <2 )
  {
    std::cout << "Need to provide mesh filename as first argument."<< std::endl;
    return 0;
  }

  std::string meshfile = std::string(argv[1]);
  igl::read_triangle_mesh(meshfile.c_str(), V, F);

  // Add the keyboard interaction
  viewer.callback_key_down = &key_down;

  // Plot the mesh
  viewer.data().clear(); // this clears all data, including overlays
  viewer.data().set_mesh(V, F); // set the mesh to display


  // Add menu boxes
  draw_menu_boxes();

  // olga's color preferences :)
  viewer.data().set_colors(.99*Eigen::RowVector3d::Ones());
  viewer.core().background_color<<1.,1.,1.,1.;
  viewer.data().line_color<<173./255,174./255,103./255,1.;

  // Launch the viewer
  viewer.launch();
  
}

void compute_bbox(const Eigen::MatrixXd &V,
                  Eigen::Vector3d &m,
                  Eigen::Vector3d &M,
                  Eigen::MatrixXd &V_box,
                  Eigen::MatrixXi &E_box)
{
  // Find the bounding box
  m = V.colwise().minCoeff();
  M = V.colwise().maxCoeff();

  // Corners of the bounding box
  V_box.resize(8,3);
  V_box <<
  m(0), m(1), m(2),
  M(0), m(1), m(2),
  M(0), M(1), m(2),
  m(0), M(1), m(2),
  m(0), m(1), M(2),
  M(0), m(1), M(2),
  M(0), M(1), M(2),
  m(0), M(1), M(2);

  // Edges of the bounding box
  E_box.resize(12,2);
  E_box <<
  0, 1,
  1, 2,
  2, 3,
  3, 0,
  4, 5,
  5, 6,
  6, 7,
  7, 4,
  0, 4,
  1, 5,
  2, 6,
  7 ,3;

}



