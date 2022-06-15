// ITK includes
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkExtractImageFilter.h>

// TCLAP includes
#include <tclap/ValueArg.h>
#include <tclap/ArgException.h>
#include <tclap/CmdLine.h>
#include <tclap/SwitchArg.h>

// STD includes
#include <cstdlib>

//NOTE: Here we assume the input and output data type is unsigned short!

int main (int argc, char **argv)
{

  // =========================================================================
  // Command-line variables
  // =========================================================================
  std::string inputImage;
  std::string outputImage;
  unsigned short ix, iy, iz; // Index
  unsigned short sx, sy, sz; // Size
  bool compressOutput;


  // =========================================================================
  // Parse arguments
  // =========================================================================
  try {

    TCLAP::CmdLine cmd("itkBoundingBox");

    TCLAP::ValueArg<std::string> input("i", "input", "Image to compute the output", true, "None", "string");
    TCLAP::ValueArg<std::string> output("o", "output", "Cropped imge", false, "None", "string");
    TCLAP::ValueArg<unsigned short> index_x("x", "index_x", "Index x", true, 0, "unsinged short");
    TCLAP::ValueArg<unsigned short> index_y("y", "index_y", "Index y", true, 0, "unsinged short");
    TCLAP::ValueArg<unsigned short> index_z("z", "index_z", "Index z", true, 0, "unsinged short");
    TCLAP::ValueArg<unsigned short> size_x("u", "size_x", "Size x", true, 0, "unsinged short");
    TCLAP::ValueArg<unsigned short> size_y("v", "size_y", "Size y", true, 0, "unsinged short");
    TCLAP::ValueArg<unsigned short> size_z("w", "size_z", "Size z", true, 0, "unsinged short");
    TCLAP::SwitchArg compress("c", "compress", "Compress output", false);

    cmd.add(input);
    cmd.add(output);
    cmd.add(index_x);
    cmd.add(index_y);
    cmd.add(index_z);
    cmd.add(size_x);
    cmd.add(size_y);
    cmd.add(size_z);
    cmd.add(compress);

    cmd.parse(argc,argv);

    inputImage = input.getValue();
    outputImage = output.getValue();
    ix = index_x.getValue();
    iy = index_y.getValue();
    iz = index_z.getValue();
    sx = size_x.getValue();
    sy = size_y.getValue();
    sz = size_z.getValue();
    compressOutput = compress.getValue();

  } catch (TCLAP::ArgException &e) {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
  }

  // =========================================================================
  // ITK definitions
  // =========================================================================
  using ImageType = itk::Image<float, 3>;
  using ImageReaderType = itk::ImageFileReader<ImageType>;
  using ImageWriterType = itk::ImageFileWriter<ImageType>;
  using ExtractImageFilterType = itk::ExtractImageFilter<ImageType, ImageType>;
  using RegionType = itk::ImageRegion<3>;

  // =========================================================================
  // Image loading and checking
  // =========================================================================
  auto inputReader = ImageReaderType::New();
  inputReader->SetFileName(inputImage);
  inputReader->Update();

  // =========================================================================
  // Set the region to extract
  // =========================================================================
  RegionType::SizeType size;
  size[0] = sx; size[1] = sy; size[2] = sz;

  RegionType::IndexType index;
  index[0] = ix; index[1] = iy; index[2] = iz;

  RegionType region(index, size);

  // =========================================================================
  // Extract the sub-volume
  // =========================================================================
  auto extractImageFilter = ExtractImageFilterType::New();
  extractImageFilter->SetInput(inputReader->GetOutput());
  extractImageFilter->SetExtractionRegion(region);
  extractImageFilter->Update();

  // =========================================================================
  // Write the sub-volume
  // =========================================================================
  if (outputImage != "None")
  {
    auto outputWriter = ImageWriterType::New();
    outputWriter->SetFileName(outputImage);
    outputWriter->SetInput(extractImageFilter->GetOutput());
    outputWriter->SetUseCompression(compressOutput);
    outputWriter->Write();
  }

  return EXIT_SUCCESS;
}
