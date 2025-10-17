#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stack>
#include <cmath>
using namespace std;
struct Point {
    double x, y;
};

// Global reference point for sorting, I use it to sort relative to this specific point, the lowest one
Point p0;

//  function to find next to top in a stack
Point nextToTop(stack<Point> &S) {
    Point top = S.top(); //
    S.pop();
    Point nextTop = S.top();
    S.push(top);
    return nextTop;
}

// function which swaps 2 points between, so I can do it faster and without thinking
void swap(Point &p1, Point &p2) {
    Point temp = p1;
    p1 = p2;
    p2 = temp;
}

// function which computes distance between 2 points squared
double distSq(Point p1, Point p2) {
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

// function to find orientation of ordered triplet
// 0 then p, q, r are collinear(they are in same line)
// 1 then oriented clockwise
// 2 then oriented counterclockwise
int orientation(Point p, Point q, Point r) {
    double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

    // Use a small epsilon for floating point comparison
    const double epsilon = 1e-10;
    if (abs(val) < epsilon) return 0;  // collinear
    return (val > 0) ? 1 : 2; // clock or counterclock wise
}

             //  function for sorting points with respect to the first point
bool compare(const Point &p1, const Point &p2) {
    // Find orientation
    int o = orientation(p0, p1, p2);
    if (o == 0)
        return (distSq(p0, p2) >= distSq(p0, p1)) ? false : true;

    return (o == 2) ? true : false;
}

       //  function for Graham's Scan from which I call all others, just makes stuff more compact when I dp
vector<Point> convexHull(vector<Point> points) {
    int n = points.size();
    if (n < 3) {
        return points; // if its 2 points its a damn straigh line so error>:{
    }

    // find the lowest point, following grahams algorithm I read from wikipedia
    double minY = points[0].y;
    int minIndex = 0;
    for (int i = 1; i < n; i++) {
        double y = points[i].y;

        // choose one with lowest c in case of a tie
        if ((y < minY) || (abs(minY - y) < 1e-10 && points[i].x < points[minIndex].x)) {
            minY = points[i].y;
            minIndex = i;
        }
    }

    // according to algorith the lowest point is our beggning
    swap(points[0], points[minIndex]);

    // now we sort with respect of the lowest point
    p0 = points[0];
    sort(points.begin() + 1, points.end(), compare);

    //  collinear points==useless, keep ones which arent in a line
    vector<Point> uniquePoints;
    uniquePoints.push_back(points[0]);

    for (int i = 1; i < n; i++) {
        // remove points until the next point has different orientation
        while (i < n - 1 && orientation(p0, points[i], points[i + 1]) == 0) {
            i++;
        }
        uniquePoints.push_back(points[i]);
    }

    int m = uniquePoints.size();
    if (m < 3) {
        return uniquePoints; // Convex hull not possible
    }

    // create an empty stack and push first three points
    stack<Point> S;
    S.push(uniquePoints[0]);
    S.push(uniquePoints[1]);
    S.push(uniquePoints[2]);

    // process remaining m-3 points
    for (int i = 3; i < m; i++) {
        // keep removing top while the angle formed by points next-to-top, and points makes a non-left turn
        while (S.size() > 1 && orientation(nextToTop(S), S.top(), uniquePoints[i]) != 2) {
            S.pop();
        }
        S.push(uniquePoints[i]);
    }

    // now stack has the output points, convert to vector
    vector<Point> hull;
    while (!S.empty()) {
        hull.push_back(S.top());
        S.pop();
    }

    // reverse to get counter-clockwise order
    reverse(hull.begin(), hull.end());

    return hull;
}

// function to read points from file
  vector<Point> readPointsFromFile(const string& filename) {
    vector<Point> points;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return points;
    }

    int numPoints;
    file >> numPoints;

    if (numPoints <= 0) {
        cerr << "Error: Invalid number of points " << numPoints << endl;
        return points;
    }

    points.reserve(numPoints);

    for (int i = 0; i < numPoints; ++i) {
        Point point;
        file >> point.x >> point.y;
        points.push_back(point);
    }

    cout << "Successfully read " << points.size() << " points from file." << endl;
    return points;
}

// Function to display points
void displayPoints(const vector<Point>& points, const string& title) {
    cout << title << " (" << points.size() << " points):" << endl;
    for (size_t i = 0; i < points.size(); ++i) {
        cout << "(" << points[i].x << ", " << points[i].y << ")";
        if (i < points.size() - 1) cout << " -> ";
    }
    cout << endl;
}
//finds shortest distance
double findShortestDistance(const vector<Point>& points, Point& p1, Point& p2) {
    if (points.size() < 2) {
        //you will try this to get me on gotcha when you interview me:>
        throw invalid_argument("you tried putting less than 2 points to try to get me didnt you?");
    }

    double minDistance = numeric_limits<double>::max();
    p1 = points[0];
    p2 = points[1];

    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = i + 1; j < points.size(); ++j) {
            // distance formula calc, I do know I can use sqrt for this,
            // but I am not sure whats more optimal, so I did this
            double dx = points[i].x - points[j].x;
            double dy = points[i].y - points[j].y;
            double dist = sqrt(dx * dx + dy * dy);

            if (dist < minDistance) {
                minDistance = dist;
                p1 = points[i];
                p2 = points[j];
            }
        }
    }

    return minDistance;
}
int main() {
    //  points from file
    vector<Point> points = readPointsFromFile(R"(C:\Users\Darkser1nt\CLionProjects\untitled1\points.txt)");

    if (points.empty()) {
        cerr << "No points to process." << endl;
        return 1;
    }

    // Find convex hull
    vector<Point> hull = convexHull(points);

    // Display convex hull points
    displayPoints(hull, "Convex Hull Points");
    Point p1, p2;
    try {
        double shortestDist = findShortestDistance(points, p1, p2);
        cout << "\nShortest Distance: " << shortestDist << endl;
        cout << "Between points: (" << p1.x << ", " << p1.y << ") and ("
                  << p2.x << ", " << p2.y << ")" << endl;
    } catch (const exception& e) {
       cerr << "Error: " << e.what() << endl;
    }

    return 0;
}