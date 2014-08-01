//
//  WordEvaluator.cpp
//  SkewDetection
//
//  Created by David Helekal on 30/05/14.
//
//

#include "WordEvaluator.h"
#include <opencv2/highgui/highgui.hpp>
namespace cmp {
    
    WordEvaluator::WordEvaluator(std::string outputDir, std::string inputDir, bool writeData, std::string *referenceFile) : outputDirectory(outputDir){
        
        wordImages = IOUtils::GetFilesInDirectory(inputDir,"*.png", true);
        
        //if the reference file is supplied, set the reference vector the given data
        if (referenceFile!=NULL)
        {
            //do something
        }
        
        //else assume skew = 0
        else
        {
            for (size_t i=0; i<wordImages.size(); i++) {
                reference.push_back(0.0);
            }
        }
        
        //get the directories containing the different words
        for (size_t i=0; i<wordImages.size(); i++) {
            directories.push_back(IOUtils::RemoveExtension(wordImages[i]));
        }
    }
    
    WordEvaluator::~WordEvaluator()
    {
        
    }
    
    void WordEvaluator::run()
    {
        createFileStructure(outputDirectory);
        
        for (size_t i=0; i<directories.size(); i++) {
            evaluateWord(directories[i], i);
        }
        saveResults(outputDirectory);
    }
    
    typedef std::map<std::string, std::map<std::string, double> >::iterator itt1;
    typedef std::map<std::string, double>::iterator itt2;
    typedef std::map<std::string, statisticalResult>::iterator itt3;
    
    static bool sortResMap(itt2 it1, itt2 it2) {return(it1->second < it2->second);};
    static bool sortCatMap(itt3 it1, itt3 it2) {return (it1->second.statMap.begin()->second < it2->second.statMap.begin()->second);};
    
    void WordEvaluator::evaluateWord(std::string wordDir, int idx)
    {
        std::cout<<wordDir;
        //vector containing lines of image data from the suplied text file
        std::vector<std::vector<std::string> > imageData;
        //the output vector
        std::vector<std::vector<ImageData> > words;
        //vector containing the path to the text file (is a vector only due to return type)
        std::string dataFilePath;
        //vector containing line data
        std::vector<std::string> lines;
        //vector containing the length of word lines
        std::vector<int> lineIndices;
        //temp string for parsing purposes
        std::string line;
        //the text file instance
        std::ifstream textdatafile;
        //index for iterating through lines
        int index=0;
        
        dataFilePath = wordDir+"/"+IOUtils::Basename(wordDir)+".txt";
        textdatafile.open(dataFilePath);
        
        while (getline(textdatafile, line)) {
            lines.push_back(line);
        }
        
        //parsing the text file
        bool isLine=false;
        for (size_t i=0; i<lines.size(); i++) {
            
            //if the line contains line data do
            if (isLine)
            {
                lineIndices.push_back(splitString(lines[i], ' ').size());
                continue;
            }
            
            //if the line contains the line header
            if (lines[i]=="LINES:")
            {
                isLine=true;
                continue;
            }
            
            //if the line contains image data
            if (!isLine && i!=0) {
                imageData.push_back(splitString(lines[i], ' '));
            }
        }
        
        //create a words vector, containing vectors with each letter
        
        for (size_t i=0; i<lineIndices.size(); i++) {
            std::vector<ImageData> temp;
            ImageData tempData;
            for (size_t i1 =0; i1 <lineIndices[i]; i1++) {
                tempData.imgName = imageData[index][5];
                tempData.letter = imageData[index][6];
                temp.push_back(tempData);
                index++;
            }
            words.push_back(temp);
        }
        
        
        for (size_t i=0; i<detectors.size(); i++) {
            
            std::string dirPath=outputDirectory+"/"+detectorIDs[i]+"/"+IOUtils::Basename(wordDir);
            IOUtils::CreateDir(dirPath);
            
            for (size_t i1=0; i1<words.size(); i1++) {
                std::vector<Blob> imgs;
                double angle;
                cv::Mat debugImage;
                double tolerance = 0.06;
                bool isWrong =true;
                std::vector<std::string> letters;
                std::string font;
                std::string imageName;
                double angleDifference;
                
                for (size_t i2=0; i2<words[i1].size(); i2++)
                {
                    cv::Mat tempImg = cv::imread(wordDir+"/"+words[i1][i2].imgName, CV_LOAD_IMAGE_GRAYSCALE);
                    tempImg = tempImg>128;
                    cv::copyMakeBorder(tempImg, tempImg, 5, 5,5,5, cv::BORDER_CONSTANT,cv::Scalar(0,0,0));
                    //cv::imshow("test", tempImg);
                    //cv::waitKey(0);
                    imgs.push_back(Blob(tempImg));
                    letters.push_back(words[i1][i2].letter);
                }
                angle=detectors[i]->detectSkew(imgs, 0.0, &debugImage);
                
                angleDifference = angle-reference[idx];
                
                if (tolerance > fabs(angleDifference)) {
                    isWrong = false;
                    
                }
                
                imageName =IOUtils::Basename(wordDir);
                
                font = splitString(imageName, '-')[0];
                
                Result tempResult(angle, angleDifference, isWrong, debugImage, imageName,letters,font);
                results.push_back(tempResult);
                
                std::stringstream imageFileName;
                imageFileName << dirPath<< "/" << imageName << i1 <<".jpeg";
                
                saveDebugImg(imageFileName.str(), tempResult);
                
            }
        }
    }
    
    //string splitting method
    
    std::vector<std::string> WordEvaluator::splitString(const std::string &inputString, char delim)
    {
        std::stringstream ss(inputString);
        std::string item;
        std::vector<std::string> outputString;
        while (std::getline(ss, item, delim)) {
            outputString.push_back(item);
        }
        return outputString;
    }
    
    void WordEvaluator::addWordDetector(cv::Ptr<WordSkewDetector> detector, std::string detectorID)
    {
        detectors.push_back(detector);
        detectorIDs.push_back(detectorID);
        
    }
    
    //function for creating the output file structure in the output folder
    
    void WordEvaluator::createFileStructure(std::string outputFolder)
    {
        //create a separate directory for storage of debug images returned by detectors with incorrect result
        std::string directoryPath;
        directoryPath=outputFolder;
        directoryPath+= "/";
        directoryPath+="Failed";
        IOUtils::CreateDir(directoryPath);
        
        //create a directory for each word
        for (size_t i=0; i<detectorIDs.size(); i++) {
            std::string directoryPath;
            directoryPath=outputFolder;
            directoryPath += "/"+detectorIDs[i];
            IOUtils::CreateDir(directoryPath);
            
        }
    }
    
    
    void WordEvaluator::createLayout(std::ofstream& outputFile){
        outputFile << "<!DOCTYPE html>" << "\n"
        <<"<html>" << "\n"
        <<"<head>" << "\n"
        <<"<style>" << "\n"
        <<"table,th,td" << "\n"
        <<"{"<< "\n"
        <<"border:"<<"1px"<<" "<< "solid"<<" "<< "black" << ";"<< "\n"
        <<"border-collapse:collapse"<< ";" << "\n"
        <<"}"<< "\n"
        <<"th,td"<< "\n"
        <<"{"<< "\n"
        <<"padding:"<<"5px"<<";"<< "\n"
        <<"</style>"<< "\n"
        <<"</head>"<< "\n";
        
    }
    //write results to the output folder & json

    void WordEvaluator::saveResults(std::string outputFolder)
    {
        std::cout << "writing results...";
        std::ofstream outputFile;
        /*outputFolder+="/Output.txt";
        outputFile.open(outputFolder);
        if (outputFile.is_open()) {
            for (size_t t=0; t<results.size(); t++) {
                if (results[t].isWrong) {
                    outputFile << results[t].imgName <<" "<< results[t].angle << " incorrect skew.\n";
                }
                else{
                    outputFile  << results[t].imgName <<" " << results[t].angle << " correct skew.\n";
                }
            }
        }
            */
        
        //preparing the file strucuture...
        std::map<std::string, Result> resultMap;
        std::vector<std::string> fontMapKeys;
        std::map<std::string, std::map<std::string, double> > fontMap;
        std::vector<std::string> letterMapKeys;
        std::map<std::string, std::map<std::string, double> > letterMap;
        std::map<std::string, std::map<std::string, std::map<std::string, double> >>catMaps;
        
        for (int i=0; i<results.size(); i++) {
            resultMap[results[i].imgName]=results[i];
        }
        
        for (auto iterator=resultMap.begin(); iterator!=resultMap.end(); ++iterator)
        {
            std::vector<std::string> letters = iterator->second.letters;
            std::string font = iterator->second.fontName;
            for (int i1 =0; i1<letters.size(); i1++)
            {
                    letterMap[letters[i1]][iterator->second.imgName] = iterator->second.angleDifference;
            }
                fontMap[font][iterator->second.imgName] = iterator->second.angleDifference;
            
            //load all the category vectors into catMaps
            catMaps["Letter"] = letterMap;
            catMaps["Font"] = fontMap;
            
        }
        
        //writing into the html file
        
        outputFile.open(outputFolder+"/"+"index.htm");
        
        createLayout(outputFile);
        
        outputFile << "<body>" << "\n";
        //itt1 iterator2;
        std::map<std::string, std::map<std::string, statisticalResult> >
        sortedCatMap;
        
        //sort all maps by descending error
        
        for (auto iterator=catMaps.begin(); iterator != catMaps.end();++iterator) {
            std::map<std::string,statisticalResult> tempStatResMap;
            std::map<std::string,statisticalResult> tempStatResMapSorted;

            for (auto iterator2 = iterator->second.begin(); iterator2!=iterator->second.end(); ++iterator2) {
                std::vector<itt2> iters;
                std::map<std::string,double> sortedTempResMap;
                std::map<std::string,double> tempStatMap;
                double mean =0.0;
                double sum = 0.0;
                int count =0;
                //sorting the individual category sub-maps
                for (auto iterator3 = iterator2->second.begin(); iterator3!=iterator2->second.end(); ++iterator3) {
                    iters.push_back(iterator3);
                }
                 std::sort(iters.begin(), iters.end(), sortResMap);
                //load values into a new, sorted map & calculate statistics
                
                for (size_t t =0 ;t<iters.size(); t++) {
                    sortedTempResMap[iters[t]->first] =iters[t]->second;
                    sum+=iters[t]->second;
                    count++;
                }
               
                if (sum!=0) {
                    mean =sum/count;
                }
                else mean =0.0;
                
                //load statistical results into the tempStatMap;
                tempStatMap["mean"] = mean;
                
                //load tempStatMap & sortedTempResMap into tempResult;
                statisticalResult tempResult(sortedTempResMap,tempStatMap);
                
                //load tempResult into tempStatResMap using the original category name as the key
                tempStatResMap[iterator2->first] = tempResult;
            }
            //sort the entire category map by descending statistical measurements (mean error in this case)
            std::vector<itt3> iters;
            for (itt3 iterator3 = tempStatResMap.begin(); iterator3 != tempStatResMap.end(); ++iterator3) {
                iters.push_back(iterator3);
            }
            std::sort(iters.begin(), iters.end(), sortCatMap);
            
            for (size_t t =0; t<iters.size(); t++){
                std::string tempString = iters[t]->first;
                auto tempSecond = iters[t]->second;
                tempStatResMapSorted[tempString] = tempSecond;
                
            }
            
            sortedCatMap[iterator->first]= tempStatResMapSorted;
            
        }
        
        //write the results
        
        for (auto iterator = sortedCatMap.begin(); iterator!= sortedCatMap.end(); ++iterator) {
             //writing the table heading
            outputFile << "<h1>" << iterator->first << "</h1>"<< "\n";
            
            //writing the table itself
            outputFile<< "<table style = \" "<< "width:500px"<< "\"" << ">" << "\n";

            outputFile << "<tr>" << "\n";
            for (auto iterator2 = iterator->second.begin(); iterator2 != iterator->second.end(); iterator2++) {
                outputFile << "<th>" << iterator2->first << "</th>";
            }
            outputFile << "</tr>" << "\n";
        }
        
        
        
        outputFile.close();
        
    }
    
    
    void WordEvaluator::saveDebugImg(std::string outputDir, cmp::Result result)
    {
        cv::imwrite(outputDir, result.debugImg);
        if(result.isWrong){
            cv::imwrite(outputDirectory+"/Failed"+"/"+result.imgName+".png", result.debugImg);
        }
   
    }
    
    
}
