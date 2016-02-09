#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkJPEGImageIOFactory.h"
#include "itkImageRegionIterator.h"
#include "QuickView.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"


#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkIdentityTransform.h"
#include "itkLabelImageGaussianInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"

#include "itkCustomColormapFunction.h"
#include "itkScalarToRGBColormapImageFilter.h"
#include "itkRGBPixel.h"
#include "itkMersenneTwisterRandomVariateGenerator.h"



#include "itkMutualInformationImageToImageMetric.h"
#include "itkRandomImageSource.h"
#include "itkTranslationTransform.h"
#include "itkLinearInterpolateImageFunction.h"


unsigned long long min(unsigned long long val1, unsigned long long val2);

int main(int, char **)
{
    char inputFile1[] = "C:/Users/bmi/Documents/GitHub/lab4/1.jpg";
    char inputFile2[] = "C:/Users/bmi/Documents/GitHub/Lab4/2.jpg";
    char inputFile3[] = "C:/Users/bmi/Documents/GitHub/Lab4/3.jpg";

    unsigned long long oneToTwoCount = 0;
    unsigned long long twoToThreeCount = 0;
    unsigned long long threeToOneCount = 0;

    unsigned char erroneous = 0;

    //////////////////////////////////
    /// Read the files
    //////////////////////////////////

    typedef itk::Image<unsigned int, 2> ImageType;
    typedef itk::ImageFileReader<ImageType> ReaderType;

    // register JPEG
    itk::JPEGImageIOFactory::RegisterOneFactory();

    ReaderType::Pointer reader1 = ReaderType::New();
    ReaderType::Pointer reader2 = ReaderType::New();
    ReaderType::Pointer reader3 = ReaderType::New();

    //read file
    reader1->SetFileName(inputFile1);
    reader2->SetFileName(inputFile2);
    reader3->SetFileName(inputFile3);


    reader1->Update();
    reader2->Update();
    reader3->Update();

    ImageType::Pointer image1 = reader1->GetOutput();
    ImageType::Pointer image2 = reader2->GetOutput();
    ImageType::Pointer image3 = reader3->GetOutput();
    ImageType::Pointer wrongImage;
    ImageType::Pointer rightImage1;
    ImageType::Pointer rightImage2;


    //////////////////////////////////
    /// Compare the three photos
    //////////////////////////////////

    ImageType::RegionType region = image1->GetLargestPossibleRegion(); // all regions should be the same
    ImageType::SizeType regionSize = region.GetSize();

    ImageType::IndexType regionIndex;
    regionIndex[0] = 0;
    regionIndex[1] = 0;

    region.SetSize(regionSize);
    region.SetIndex(regionIndex);

    itk::ImageRegionIterator<ImageType> imageIterator1(image1, region);
    itk::ImageRegionIterator<ImageType> imageIterator2(image2, region);
    itk::ImageRegionIterator<ImageType> imageIterator3(image3, region);

    imageIterator1.GoToBegin();
    imageIterator2.GoToBegin();
    imageIterator3.GoToBegin();

    while(!imageIterator1.IsAtEnd())
    {
      // Get the value of the current pixel
      //unsigned char val = imageIterator.Get();
      //std::cout << (int)val << std::endl;

      // Set the current pixel to white
      //imageIterator.Set(255);

      //divide by 2 so that the number doesn't increment as fast, and you don't run out of space
      oneToTwoCount += std::abs(imageIterator2.Get() - imageIterator1.Get());
      twoToThreeCount += std::abs(imageIterator3.Get() - imageIterator2.Get());
      threeToOneCount += std::abs(imageIterator1.Get() - imageIterator3.Get());

      if (oneToTwoCount > 9223372036854770000 || twoToThreeCount > 9223372036854770000
              || threeToOneCount > 9223372036854770000){
          std::cout << "Space is running low! Results might not be correct!" ;
      }


      ++imageIterator1;
      ++imageIterator2;
      ++imageIterator3;
    }

    std::cout << "Image 1 - 2: " << oneToTwoCount << "\n";
    std::cout << "Image 2 - 3: " << twoToThreeCount << "\n";
    std::cout << "Image 3 - 1: " << threeToOneCount << "\n";

    //////////////////////////////////
    /// Determine which is erroneous
    //////////////////////////////////

    unsigned long long minCount = min(oneToTwoCount, twoToThreeCount);
    minCount = min(minCount, threeToOneCount);

    if (oneToTwoCount == minCount){
        std::cout << "Three is erroneous";
        erroneous = 3;
        wrongImage = image3;
        rightImage1 = image1;
        rightImage2 = image2;
    }
    else if (twoToThreeCount == minCount){
        std::cout << "One is erroneous";
        erroneous = 1;
        wrongImage = image1;
        rightImage1 = image2;
        rightImage2 = image3;
    }
    else if (threeToOneCount == minCount){
        std::cout << "Two is erroneous";
        erroneous = 2;
        wrongImage = image2;
        rightImage1 = image1;
        rightImage2 = image3;
    }
    else {
        std::cout << "Something went wrong...";
        return 1;
    }


    //////////////////////////////////
    /// Interpolate new photo
    //////////////////////////////////

    typedef itk::MutualInformationImageToImageMetric<ImageType, ImageType >    MetricType;

    typedef itk::TranslationTransform<double, 2> TranslationTransformType; // This cannot be float for some reason?
      TranslationTransformType::Pointer transform = TranslationTransformType::New();

    MetricType::Pointer metric = MetricType::New();

    metric->SetTransform(transform);

 //   metric->SetFixedImageStandardDeviation(  4 );
 //   metric->SetMovingImageStandardDeviation( 4 );

    metric->SetFixedImage(fixedImage);
    metric->SetMovingImage(movingImage);

    metric->SetFixedImageRegion(fixedImage->GetLargestPossibleRegion());

    itk::LinearInterpolateImageFunction<ImageType, double>::Pointer interpolator = itk::LinearInterpolateImageFunction<ImageType, double>::New();
    interpolator->SetInputImage(fixedImage);
    metric->SetInterpolator(interpolator);


 /*   itk::ImageRegionIterator<ImageType> wrongIterator(wrongImage, region); //region is same
    itk::ImageRegionIterator<ImageType> rightIterator1(rightImage1, region);
    itk::ImageRegionIterator<ImageType> rightIterator2(rightImage2, region);

    wrongIterator.GoToBegin();
    rightIterator1.GoToBegin();
    rightIterator2.GoToBegin();

    while(!wrongIterator.IsAtEnd())
    {

      wrongIterator.Set((rightIterator2.Get() + rightIterator1.Get()) / 2);

      ++wrongIterator;
      ++rightIterator1;
      ++rightIterator2;
    }
*/
    //////////////////////////////////
    /// Display results
    //////////////////////////////////


    QuickView viewer;

    if (erroneous == 1){
        viewer.AddImage(resizeFilter2->GetOutput(),//image1.GetPointer(),
                        true,
                        "This image is interpolated");
        viewer.AddImage(image2.GetPointer());
        viewer.AddImage(image3.GetPointer());

    }
    else if (erroneous == 2){

        viewer.AddImage(image1.GetPointer());
        viewer.AddImage(resizeFilter2->GetOutput(),//image2.GetPointer(),
                        true,
                        "This image is interpolated");
        viewer.AddImage(image3.GetPointer());

    }
    else if (erroneous == 3){

        viewer.AddImage(image1.GetPointer());
        viewer.AddImage(image2.GetPointer());
        viewer.AddImage(metric->GetOutput(),//image3.GetPointer(),
                        true,
                        "This image is interpolated");
    }
    else {
        std::cout << "Something went wrong...";
        return 1;
    }

    viewer.Visualize();


    return 0;
}

unsigned long long min(unsigned long long val1, unsigned long long val2){
    if (val1 > val2){
        return val2;
    }
    return val1;
}
