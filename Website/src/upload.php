<?php
ini_set('display_errors',1);
error_reporting(E_ALL | E_STRICT);
mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
//Establish Connection -- mysqli_connect(host,username,password,database_name)
require("db.php");


$target_dir = "uploads/";
$target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
$uploadOk = 1;
$imageFileType = strtolower(pathinfo($target_file,PATHINFO_EXTENSION));
$fileSize = $_FILES["fileToUpload"]["size"] ;

echo $fileSize;
// Check if image file is a actual image or fake image
if(isset($_POST["submit"])) {
  $fileName = $_FILES["fileToUpload"]["tmp_name"];
  echo $fileName;
  $check = getimagesize($fileName);
  
  echo "File is an image - " . $check["mime"] . ".";
  $uploadOk = 1;
    //FileStyle PATIENTID-MMDDYYYY
    //File style: XXXX-05012023.png
  $FileNameNoExtension = substr($fileName, 0, -4);
  $FileInfo = explode("-", $FileNameNoExtension);  
  $PatientID = $FileInfo[0];
  $DateAQ = substr($FileInfo[1],0,0);
  
  $md5sum = md5_file($fileName);
  $query = "INSERT INTO Test1 (PatientID, AcquisitionDate, Path, Md5sum, FileSize) VALUES ($PatientID, $DateAQ, $newPath, $md5sum, $fileSize)";
  $result = mysqli_query($conn, $query);
}

// Check if file already exists
if (file_exists($target_file)) {
  echo "Sorry, file already exists.";
  $uploadOk = 0;
}

// Check file size
if ($fileSize > 500000) {
  
  echo "Sorry, your file is too large.";
  $uploadOk = 0;
}

// Allow certain file formats
if($imageFileType != "jpg" && $imageFileType != "png" && $imageFileType != "jpeg"
&& $imageFileType != "gif" ) {
  echo "Sorry, only JPG, JPEG, PNG & GIF files are allowed.";
  $uploadOk = 0;
}

// Check if $uploadOk is set to 0 by an error
if ($uploadOk == 0) {
  echo "Sorry, your file was not uploaded.";
// if everything is ok, try to upload file
} else {
  if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file)) {
    echo "The file ". htmlspecialchars( basename( $_FILES["fileToUpload"]["name"])). " has been uploaded.";
  } else {
    echo "Sorry, there was an error uploading your file.";
  }
}
?>