#ifndef RN_FEATURES_HXX
#define RN_FEATURES_HXX

#include <vigra/multi_array.hxx>

namespace vigra
{

template<class T, class C = StridedArrayTag>
class FeatureBase
{
public:
    MultiArrayView<2, T, C> original_;
    MultiArrayShape<2>::type im_shape_;      // shape of original image (i.e. # of pixels in x,y)

    FeatureBase(MultiArrayView<2, T, C> const & original,
                MultiArrayShape<2>::type  im_shape)
        : original_(original), im_shape_(im_shape)
    {}

    virtual ~FeatureBase() { }

    MultiArrayShape<2>::type shape() const
    {
        MultiArrayShape<2>::type(original_.size(0), original_.size(1));
    }

    int shape(int n) const
    {
        return shape()[n];
    }

    virtual T operator() (int i, int j) const = 0;
};

template<class T, class C = StridedArrayTag>
class NormalFeatures : public FeatureBase<T,C>
{
public:
    typedef FeatureBase<T,C>    FB;

    NormalFeatures(MultiArrayView<2, T, C> const & original,
                   MultiArrayShape<2>::type  im_shape)
        : FB(original, im_shape)
    {}

    ~NormalFeatures() { }

    T operator() (int i, int j) const
    {
        return FB::original_(i, j);
    }

};

template<class T, class C = StridedArrayTag>
class OffsetFeatures : public FeatureBase<T,C>
{
public:
    typedef FeatureBase<T,C>    FB;

    int offset_x_, offset_y_;

    OffsetFeatures(MultiArrayView<2, T, C> const & original,
                   MultiArrayShape<2>::type im_shape,
                   int offset_x,
                   int offset_y)
        : FB(original, im_shape)
    {
        offset_x_ = offset_x;
        offset_y_ = offset_y;
    }

    ~OffsetFeatures() { }

    T operator() (int i, int j) const
    {
        // there may be multiple images passed in simultaneously, appended into a big feature array.  calc corresponding offset and apply.
        int L = FB::im_shape_[0]*FB::im_shape_[1];
        int im_offset = static_cast<int>(floor(i/L))*L;
        i -= im_offset;

        // convert index i (sample #) to x,y position in image, and then shift
        int x = i % FB::im_shape_[0];
        int y = static_cast<int>(floor(i/FB::im_shape_[0]));
        int xp = x + offset_x_;
        int yp = y + offset_y_;

        // deal with out of bounds indices.  just move them back to the border of the image.
        // IN FUTURE, DO SOMETHING SMARTER HERE.  E.G., REFLECT IMAGE OVER THE BORDER?
        if (xp >= FB::im_shape_[0]) xp = FB::im_shape_[0]-1;
        else if (xp < 0) xp = 0;
        if (yp >= FB::im_shape_[1]) yp = FB::im_shape_[1]-1;
        else if (yp < 0) yp = 0;

        // convert back to index into feature array, and correct for initial offset.  note: i is not corrected, b/c not used again.
        int ip = yp * FB::im_shape_[0] + xp;
        ip += im_offset;

        // return
        return FB::original_(ip, j);
    }

};

template<class T, class C = StridedArrayTag>
class DiffFeatures : public FeatureBase<T,C>
{
public:
    typedef FeatureBase<T,C>    FB;

    int offset_x_, offset_y_;

    DiffFeatures(MultiArrayView<2, T, C> const & original,
                   MultiArrayShape<2>::type im_shape,
                   int offset_x,
                   int offset_y)
        : FB(original, im_shape)
    {
        offset_x_ = offset_x;
        offset_y_ = offset_y;
    }

    ~DiffFeatures() { }

    T operator() (int i, int j) const
    {
        // there may be multiple images passed in simultaneously, appended into a big feature array.  calc corresponding offset and apply.
        int L = FB::im_shape_[0]*FB::im_shape_[1];
        int im_offset = static_cast<int>(floor(i/L))*L;
        i -= im_offset;

        // convert index i (sample #) to x,y position in image, and then shift
        int x = i % FB::im_shape_[0];
        int y = static_cast<int>(floor(i/FB::im_shape_[0]));
        int xp = x + offset_x_;
        int yp = y + offset_y_;

        // deal with out of bounds indices.  just move them back to the border of the image.
        // IN FUTURE, DO SOMETHING SMARTER HERE.  E.G., REFLECT IMAGE OVER THE BORDER?
        if (xp >= FB::im_shape_[0]) xp = FB::im_shape_[0]-1;
        else if (xp < 0) xp = 0;
        if (yp >= FB::im_shape_[1]) yp = FB::im_shape_[1]-1;
        else if (yp < 0) yp = 0;

        // convert back to index into feature array, and correct for initial offset
        int ip = yp * FB::im_shape_[0] + xp;
        ip += im_offset;
        i += im_offset;

        // return
        return FB::original_(ip, j) - FB::original_(i, j);
    }
};


}

#endif
