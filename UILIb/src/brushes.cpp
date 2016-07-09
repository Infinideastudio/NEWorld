#include <UILIb.h>
namespace UI
{
    namespace Graphics
    {
        namespace Brushes
        {
            SolidColorBrush::SolidColorBrush(const Color & _col):
                col(_col)
            {
            }

            Color SolidColorBrush::sample(double x, double y)
            {
                return col;
            }

            GradientBrush::GradientBrush(const Color & _col0, const Color & _col1, const Color & _col2, const Color & _col3):
                col0(_col0), col1(_col1), col2(_col2), col3(_col3)
            {
            }

            Color GradientBrush::sample(double x, double y)
            {
                return Color();
            }
        }
    }
}
