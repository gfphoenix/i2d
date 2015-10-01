#include <rectpacking.hpp>
#include <stdint.h>

class Pack1 : public Packer
{
    struct Node
    {
        Node *left;
        Node *right;
        Rect r;
        Node():left(nullptr), right(nullptr),r(){}
        ~Node()
        {
            if(left)
                delete left;
            if(right)
                delete right;
        }
    };
    Node root_;
    //std::vector<Node> nodes;
    public:
    virtual ~Pack1(){}
    Pack1(){}
    void init(int w, int h) override;
    virtual float getOccupancy()const override;
    // no rotate implementation
    virtual Rect insert(int w, int h, bool mayRotate)override;
    protected:
    Node *insert(Node *root, int w, int h);
    unsigned long UsedSurfaceArea(const Node &node) const;
};

void Pack1::init(int w, int h)
{
    Packer::init(w,h);
    root_.r.x=root_.r.y=0;
    root_.r.width = w;
    root_.r.height = h;
    root_.left=root_.right=nullptr;
    //nodes.clear();
}
float Pack1::getOccupancy() const
{
    unsigned long totalSurfaceArea = imageWidth * imageHeight;
    unsigned long usedSurfaceArea = UsedSurfaceArea(root_);
    return (float)usedSurfaceArea/totalSurfaceArea;
}
unsigned long Pack1::UsedSurfaceArea(const Node &node) const
{
    if (node.left || node.right)
    {
        unsigned long usedSurfaceArea = node.r.width * node.r.height;
        if (node.left)
            usedSurfaceArea += UsedSurfaceArea(*node.left);
        if (node.right)
            usedSurfaceArea += UsedSurfaceArea(*node.right);

        return usedSurfaceArea;
    }

    // This is a leaf node, it doesn't constitute to the total surface area.
    return 0;
}
Rect Pack1::insert(int width, int height, bool)
{
    auto node = insert(&root_, width, height);
    if(node)
        return node->r;
    return Rect();
}
Pack1::Node *Pack1::insert(Pack1::Node *node, int width, int height)
{
    // If this node is an internal node, try both leaves for possible space.
    // (The rectangle in an internal node stores used space, the leaves store free space)
    if (node->left || node->right)
    {
        if (node->left)
        {
            Node *newNode = insert(node->left, width, height);
            if (newNode)
                return newNode;
        }
        if (node->right)
        {
            Node *newNode = insert(node->right, width, height);
            if (newNode)
                return newNode;
        }
        return nullptr; // Didn't fit into either subtree!
    }

    // This node is a leaf, but can we fit the new rectangle here?
    if (width > node->r.width || height > node->r.height)
        return nullptr; // Too bad, no space.

    // The new cell will fit, split the remaining space along the shorter axis,
    // that is probably more optimal.
    int w = node->r.width - width;
    int h = node->r.height - height;
    //nodes.push_back(Node()); node->left = &nodes.back();
    //nodes.push_back(Node()); node->right = &nodes.back();
    node->left = new Node();
    node->right = new Node();
    if (w <= h) // Split the remaining space in horizontal direction.
    {
        node->left->r.x = node->r.x + width;
        node->left->r.y = node->r.y;
        node->left->r.width = w;
        node->left->r.height = height;

        node->right->r.x = node->r.x;
        node->right->r.y = node->r.y + height;
        node->right->r.width = node->r.width;
        node->right->r.height = h;
    }
    else // Split the remaining space in vertical direction.
    {
        node->left->r.x = node->r.x;
        node->left->r.y = node->r.y + height;
        node->left->r.width = width;
        node->left->r.height = h;

        node->right->r.x = node->r.x + width;
        node->right->r.y = node->r.y;
        node->right->r.width = w;
        node->right->r.height = node->r.height;
    }
    // Note that as a result of the above, it can happen that node->left or node->right
    // is now a degenerate (zero area) rectangle. No need to do anything about it,
    // like remove the nodes as "unnecessary" since they need to exist as children of
    // this node (this node can't be a leaf anymore).

    // This node is now a non-leaf, so shrink its area - it now denotes
    // *occupied* space instead of free space. Its children spawn the resulting
    // area of free space.
    node->r.width = width;
    node->r.height = height;
    return node;
}
struct Rect2 : public  Rect
{
    int score1;
    int score2;
};

class Pack2 : public Packer
{
    protected:
        std::vector<Rect2> freeRects;
        std::vector<Rect2> usedRects;
        virtual float getOccupancy()const;
        virtual Rect insert(int w, int h, bool mayRotate);
        static inline int abs(int x){return x<0 ? -x : x;}
        static inline float abs(float x){return x<0?-x:x;}
        static inline int min(int a,int b){return a<b?a:b;}
        static inline float min(float a, float b){return a<b?a:b;}
        static inline int max(int a, int b){return a<b?b:a;}
        static inline float max(float a, float b){return a<b?b:a;}
        Rect2 _findPositionForNewNodeBottomLeft (int width, int height,bool rotate);
        Rect2 _findPositionForNewNodeBestShortSideFit (int width, int height, bool rotate);
        Rect2 _findPositionForNewNodeBestLongSideFit (int width, int height, bool rotate);
        Rect2 _findPositionForNewNodeBestAreaFit (int width, int height, bool rotate);
        Rect2 _findPositionForNewNodeContactPoint(int width, int height, bool rotate);

    private:
        int contactPointScoreNode (int x, int y, int width, int height);
        static inline int commonIntervalLength (int i1start, int i1end, int i2start, int i2end) {
            if (i1end < i2start || i2end < i1start) return 0;
            return min(i1end, i2end) - max(i1start, i2start);
        }
        static inline bool isContainedIn (const Rect2 &a, const Rect2 &b) {
            return a.x >= b.x && a.y >= b.y
                && a.x + a.width <= b.x + b.width
                && a.y + a.height <= b.y + b.height;
        }
        bool splitFreeNode (const Rect2 &freeNode, const Rect2 &usedNode);
        void pruneFreeList ();
        void placeRect (const Rect2 &node);
};

float Pack2::getOccupancy()const
{
    int usedSurfaceArea = 0;
    for (auto &r : usedRects)
        usedSurfaceArea += r.width * r.height;
    return (float)usedSurfaceArea / (imageWidth * imageHeight);
}
Rect Pack2::insert(int w, int h, bool mayRotate)
{
    return Rect();
}
Rect2 Pack2::_findPositionForNewNodeBottomLeft (int width, int height, bool rotate)
{
    Rect2 bestNode;
    bestNode.score1 = INT32_MAX;
    for(auto &x : freeRects){
        if(x.width>=width && x.height>=height){
            int topSideY = x.y + height;
            if(topSideY<bestNode.score1 || (topSideY==bestNode.score1 && x.x<bestNode.score2)){
                bestNode.x = x.x;
                bestNode.y = x.y;
                bestNode.width = width;
                bestNode.height = height;
                bestNode.score1 = topSideY;
                bestNode.score2 = x.x;
                bestNode.rotate = false;
            }
        }
        if(rotate && x.width>=height && x.height >= width){
            int rw = height;
            int rh = width;
            int topSideY = x.y + rh;
            if (topSideY < bestNode.score1 || (topSideY == bestNode.score1 && x.x < bestNode.score2)) {
                bestNode.x = x.x;
                bestNode.y = x.y;
                bestNode.width = rw;
                bestNode.height = rh;
                bestNode.score1 = topSideY;
                bestNode.score2 = x.x;
                bestNode.rotate = true;
            }
        }
    }

    return bestNode;
}
Rect2 Pack2::_findPositionForNewNodeBestShortSideFit (int width, int height, bool rotate) {
    Rect2 bestNode;
    bestNode.score1 = INTMAX_MAX;

    for (auto &x : freeRects) {
        // Try to place the rectangle in upright (non-rotated) orientation.
        if (x.width >= width && x.height >= height) {
            int leftoverHoriz = abs(x.width - width);
            int leftoverVert = abs(x.height - height);
            int shortSideFit = min(leftoverHoriz, leftoverVert);
            int longSideFit = max(leftoverHoriz, leftoverVert);

            if (shortSideFit < bestNode.score1 || (shortSideFit == bestNode.score1 && longSideFit < bestNode.score2)) {
                bestNode.x = x.x;
                bestNode.y = x.y;
                bestNode.width = width;
                bestNode.height = height;
                bestNode.score1 = shortSideFit;
                bestNode.score2 = longSideFit;
                bestNode.rotate = false;
            }
        }
        int rotatedWidth = height;
        int rotatedHeight = width;
        if (rotate && x.width >= rotatedWidth && x.height >= rotatedHeight) {
            int flippedLeftoverHoriz = abs(x.width - rotatedWidth);
            int flippedLeftoverVert = abs(x.height - rotatedHeight);
            int flippedShortSideFit = min(flippedLeftoverHoriz, flippedLeftoverVert);
            int flippedLongSideFit = max(flippedLeftoverHoriz, flippedLeftoverVert);

            if (flippedShortSideFit < bestNode.score1
                    || (flippedShortSideFit == bestNode.score1 && flippedLongSideFit < bestNode.score2)) {
                bestNode.x = x.x;
                bestNode.y = x.y;
                bestNode.width = rotatedWidth;
                bestNode.height = rotatedHeight;
                bestNode.score1 = flippedShortSideFit;
                bestNode.score2 = flippedLongSideFit;
                bestNode.rotate = true;
            }
        }
    }

    return bestNode;
}
Rect2 Pack2::_findPositionForNewNodeBestLongSideFit (int width, int height, bool rotate) {
    Rect2 bestNode;
    bestNode.score2 = INTMAX_MAX;
    int rotatedWidth=height;
    int rotatedHeight=width;
    for (auto x : freeRects) {
        // Try to place the rectangle in upright (non-rotated) orientation.
        if (x.width >= width && x.height >= height) {
            int leftoverHoriz = abs(x.width - width);
            int leftoverVert = abs(x.height - height);
            int shortSideFit = min(leftoverHoriz, leftoverVert);
            int longSideFit = max(leftoverHoriz, leftoverVert);

            if (longSideFit < bestNode.score2 || (longSideFit == bestNode.score2 && shortSideFit < bestNode.score1)) {
                bestNode.x = x.x;
                bestNode.y = x.y;
                bestNode.width = width;
                bestNode.height = height;
                bestNode.score1 = shortSideFit;
                bestNode.score2 = longSideFit;
                bestNode.rotate = false;
            }
        }

        if (rotate && x.width >= rotatedWidth && x.height >= rotatedHeight) {
            int leftoverHoriz = abs(x.width - rotatedWidth);
            int leftoverVert = abs(x.height - rotatedHeight);
            int shortSideFit = min(leftoverHoriz, leftoverVert);
            int longSideFit = max(leftoverHoriz, leftoverVert);

            if (longSideFit < bestNode.score2 || (longSideFit == bestNode.score2 && shortSideFit < bestNode.score1)) {
                bestNode.x = x.x;
                bestNode.y = x.y;
                bestNode.width = rotatedWidth;
                bestNode.height = rotatedHeight;
                bestNode.score1 = shortSideFit;
                bestNode.score2 = longSideFit;
                bestNode.rotate = true;
            }
        }
    }
    return bestNode;
}
Rect2 Pack2::_findPositionForNewNodeBestAreaFit (int width, int height, bool rotate) {
    int rotatedWidth = height;
    int rotatedHeight = width;
    Rect2 bestNode;

    bestNode.score1 = INT32_MAX; // best area fit, score2 is best short side fit

    for (auto &x : freeRects) {
        int areaFit = x.width * x.height - width * height;

        // Try to place the rectangle in upright (non-rotated) orientation.
        if (x.width >= width && x.height >= height) {
            int leftoverHoriz = abs(x.width - width);
            int leftoverVert = abs(x.height - height);
            int shortSideFit = min(leftoverHoriz, leftoverVert);

            if (areaFit < bestNode.score1 || (areaFit == bestNode.score1 && shortSideFit < bestNode.score2)) {
                bestNode.x = x.x;
                bestNode.y = x.y;
                bestNode.width = width;
                bestNode.height = height;
                bestNode.score2 = shortSideFit;
                bestNode.score1 = areaFit;
                bestNode.rotate = false;
            }
        }

        if (rotate && x.width >= rotatedWidth && x.height >= rotatedHeight) {
            int leftoverHoriz = abs(x.width - rotatedWidth);
            int leftoverVert = abs(x.height - rotatedHeight);
            int shortSideFit = min(leftoverHoriz, leftoverVert);

            if (areaFit < bestNode.score1 || (areaFit == bestNode.score1 && shortSideFit < bestNode.score2)) {
                bestNode.x = x.x;
                bestNode.y = x.y;
                bestNode.width = rotatedWidth;
                bestNode.height = rotatedHeight;
                bestNode.score2 = shortSideFit;
                bestNode.score1 = areaFit;
                bestNode.rotate = true;
            }
        }
    }
    return bestNode;
}
Rect2 Pack2::_findPositionForNewNodeContactPoint (int width, int height, bool rotate) {
    int rotatedWidth = height;
    int rotatedHeight = width;
    Rect2 bestNode;

    bestNode.score1 = -1; // best contact score

    for (auto &x : freeRects) {
        // Try to place the rectangle in upright (non-rotated) orientation.
        if (x.width >= width && x.height >= height) {
            int score = contactPointScoreNode(x.x, x.y, width, height);
            if (score > bestNode.score1) {
                bestNode.x = x.x;
                bestNode.y = x.y;
                bestNode.width = width;
                bestNode.height = height;
                bestNode.score1 = score;
                bestNode.rotate = false;
            }
        }
        if (rotate && x.width >= rotatedWidth && x.height >= rotatedHeight) {
            // This was width,height -- bug fixed?
            int score = contactPointScoreNode(x.x, x.y, rotatedWidth, rotatedHeight);
            if (score > bestNode.score1) {
                bestNode.x = x.x;
                bestNode.y = x.y;
                bestNode.width = rotatedWidth;
                bestNode.height = rotatedHeight;
                bestNode.score1 = score;
                bestNode.rotate = true;
            }
        }
    }
    return bestNode;
}

int Pack2::contactPointScoreNode (int x, int y, int width, int height) {
    int score = 0;
    if (x == 0 || x + width == imageWidth) score += height;
    if (y == 0 || y + height == imageHeight) score += width;

    for (unsigned i = 0; i < usedRects.size(); i++) {
        auto &r = usedRects[i];
        if (r.x == x + width || r.x + r.width == x)
            score += commonIntervalLength(r.y, r.y + r.height, y,
                    y + height);
        if (r.y == y + height || r.y + r.height == y)
            score += commonIntervalLength(r.x, r.x + r.width, x, x
                    + width);
    }
    return score;
}
bool Pack2::splitFreeNode (const Rect2 &freeNode, const Rect2 &usedNode)
{
    // Test with SAT if the rectangles even intersect.
    if (usedNode.x >= freeNode.x + freeNode.width || usedNode.x + usedNode.width <= freeNode.x
            || usedNode.y >= freeNode.y + freeNode.height || usedNode.y + usedNode.height <= freeNode.y) return false;

    if (usedNode.x < freeNode.x + freeNode.width && usedNode.x + usedNode.width > freeNode.x) {
        // New node at the top side of the used node.
        if (usedNode.y > freeNode.y && usedNode.y < freeNode.y + freeNode.height) {
            Rect2 newNode = freeNode;
            newNode.height = usedNode.y - newNode.y;
            freeRects.push_back(newNode);
        }
        // New node at the bottom side of the used node.
        if (usedNode.y + usedNode.height < freeNode.y + freeNode.height) {
            Rect2 newNode = freeNode;
            newNode.y = usedNode.y + usedNode.height;
            newNode.height = freeNode.y + freeNode.height - (usedNode.y + usedNode.height);
            freeRects.push_back(newNode);
        }
    }

    if (usedNode.y < freeNode.y + freeNode.height && usedNode.y + usedNode.height > freeNode.y) {
        // New node at the left side of the used node.
        if (usedNode.x > freeNode.x && usedNode.x < freeNode.x + freeNode.width) {
            Rect2 newNode = freeNode;
            newNode.width = usedNode.x - newNode.x;
            freeRects.push_back(newNode);
        }
        // New node at the right side of the used node.
        if (usedNode.x + usedNode.width < freeNode.x + freeNode.width) {
            Rect2 newNode = freeNode;
            newNode.x = usedNode.x + usedNode.width;
            newNode.width = freeNode.x + freeNode.width - (usedNode.x + usedNode.width);
            freeRects.push_back(newNode);
        }
    }
    return true;
}
void Pack2::pruneFreeList ()
{
    /*
     * /// Would be nice to do something like this, to avoid a Theta(n^2) loop through each pair. /// But unfortunately it
     * doesn't quite cut it, since we also want to detect containment. /// Perhaps there's another way to do this faster than
     * Theta(n^2).
     *
     * if (freeRectangles.size > 0) clb::sort::QuickSort(&freeRectangles[0], freeRectangles.size, NodeSortCmp);
     *
     * for(int i = 0; i < freeRectangles.size-1; i++) if (freeRectangles[i].x == freeRectangles[i+1].x && freeRectangles[i].y
     * == freeRectangles[i+1].y && freeRectangles[i].width == freeRectangles[i+1].width && freeRectangles[i].height ==
     * freeRectangles[i+1].height) { freeRectangles.erase(freeRectangles.begin() + i); --i; }
     */

    // / Go through each pair and remove any rectangle that is redundant.
    for (unsigned i = 0; i < freeRects.size(); i++)
        for (unsigned j = i + 1; j < freeRects.size(); ++j) {
            if (isContainedIn(freeRects[i], freeRects[j])) {
                freeRects.erase(freeRects.begin()+i);
                --i;
                break;
            }
            if (isContainedIn(freeRects[i], freeRects[j])) {
                freeRects.erase(freeRects.begin()+j);
                --j;
            }
        }
}
void Pack2::placeRect (const Rect2 &node) {
    int numRectanglesToProcess = freeRects.size();
    for (int i = 0; i < numRectanglesToProcess; i++) {
        if (splitFreeNode(freeRects[i], node)) {
            freeRects.erase(freeRects.begin()+i);
            --i;
            --numRectanglesToProcess;
        }
    }

    pruneFreeList();
    usedRects.push_back(node);
}
