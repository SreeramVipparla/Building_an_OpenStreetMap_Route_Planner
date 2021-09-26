#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

  start_node = &m_Model.FindClosestNode(start_x, start_y);
  end_node = &m_Model.FindClosestNode(end_x, end_y);
}

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
 return node->distance(*end_node);
}

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
	current_node->FindNeighbors();
  	
    for (auto neighbor : current_node->neighbors) {
      	neighbor->parent = current_node;
      	neighbor->g_value = current_node->g_value + current_node->distance(*neighbor);
      	neighbor->h_value = CalculateHValue(neighbor);
      	neighbor->visited = true;
      	open_list.push_back(neighbor);
    }
}

RouteModel::Node *RoutePlanner::NextNode() {    
std::sort(open_list.begin(), open_list.end(), [](const RouteModel::Node* a, const RouteModel::Node* b) -> bool
    {
        return a->g_value + a->h_value > b->g_value + b->h_value;
    });
    RouteModel::Node * next_node = open_list.back();
    open_list.pop_back();
    return next_node;

}
std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;
    while (current_node->parent != nullptr) {
    path_found.push_back(*current_node);
    distance += current_node->distance(*current_node->parent);
    current_node = current_node->parent;
  }
  path_found.push_back(*current_node);
   std::reverse(path_found.begin(), path_found.end());

    distance *= m_Model.MetricScale(); 
    return path_found;

}
void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = nullptr;
    
    open_list.push_back(start_node);
    start_node->visited = true;

    while (!open_list.empty() ) {
      current_node = NextNode();
 

      if (current_node == end_node) {
        m_Model.path = ConstructFinalPath(current_node);
        return ;
      }

      AddNeighbors(current_node);
    }
}
   
