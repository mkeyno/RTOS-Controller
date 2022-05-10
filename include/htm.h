#ifndef _HTML_H
#define _HTML_H

const char config_html[] PROGMEM =  R"rawliteral(
 
 
<!DOCTYPE html>
<html>
<head>
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<script>
		var MyIP=location.host;
		var _Connection;
		var link="ws://"+MyIP+"/ws";

		window.onload = function (){  
		
		updateWifiStatus(); 
		} 
		function ini_ws(){
		_Connection = new WebSocket( link); 
		_Connection.onerror = function (error) {  
							document.getElementById('_link').style.backgroundColor ="#FFA500";
							alert('Conection Error '+'\n'+link);
							}
		_Connection.close = function (error) {  document.getElementById('_link').style.backgroundColor ="#FFE4E1";} //gray
		_Connection.onopen = function (evt) {  
						  var newdate=new Date();
						  var C_date=pad2(newdate.getFullYear())+pad2(newdate.getDay()) +pad2(newdate.getDate()) ;
						  var C_sec=newdate.getSeconds()+ newdate.getMinutes()*60 + newdate.getHours()*3600; 
						  _Connection.send('SGT=' + C_date + C_sec); //15
						  document.getElementById('_link').style.backgroundColor ="#7FFF00"; // grenn
						  document.getElementById('current_data').value=newdate;
						  }
		_Connection.onmessage = function(INCOME){parsing(INCOME.data); }     
		}
		
		function parsing(_income) {// alert(_income) 
		 // document.getElementById('_content').value=_income;
		 }
		
		function updateWifiStatus() {
				var _AJX=createXHR();    
				_AJX.onreadystatechange = function(){if(_AJX.readyState == 4){
																				var recieved =_AJX.responseText; 
																				console.log(recieved);
																				pars_wifi_info(recieved);}}
				console.log("/updateWifiStatus?code=UWS");
				_AJX.open("GET","/process?code=UWS", true);
				_AJX.send(null);
				}
		function pars_wifi_info(recieved){
			var lp=recieved.split("%");
			document.getElementById('thisIP').innerHTML=lp[0];
			document.getElementById('thisNet').innerHTML=lp[1];
			document.getElementById('thisMode').innerHTML=lp[2];
			}
		function pad2(number) { return (number < 10 ? '0' : '') + number }
		
		function createXHR(){
		try{return new XMLHttpRequest();}catch (e){try {return new ActiveXObject("Microsoft.XMLHTTP");} 
		catch (e){return new ActiveXObject("Msxml2.XMLHTTP");}}
		}
		function show_list_file() {
		   var _AJX=createXHR();    
		   _AJX.onreadystatechange = function(){if(_AJX.readyState == 4){ 
		                           var rec =  _AJX.responseText;
								   document.getElementById('showfiles').innerHTML =rec;  
								   console.log(rec);		
								   var _listBox=document.getElementById('listfile');
								       _listBox.selectedIndex=_listBox.options.length-1;                        
								   }
		   }
		   console.log("/show_list_file?code=DIR");
		   _AJX.open("GET","/process?code=DIR" , true);
		   _AJX.send(null); 

		}
		
		function show_file_content(){
			var patch= document.getElementById('_path').value ;
			var file= document.getElementById('_file').value;  
			var _AJX=createXHR();    
			_AJX.onreadystatechange = function(){if(_AJX.readyState == 4){
																		  var recieved =_AJX.responseText;	document.getElementById('_content').value=recieved; }}
			console.log("/show_file_content?code=SFC&patch=" + patch+ '&file='+file);
			_AJX.open("GET","/process?code=SFC&patch=" + patch+ '&file='+file, true);
			_AJX.send(null); 
			}
		function all_information(){
			var _AJX=createXHR();    
			_AJX.onreadystatechange = function(){if(_AJX.readyState == 4){
																		  var recieved =_AJX.responseText;	document.getElementById('_content').value=recieved; }}
			console.log("/all_information?code=INF");
			_AJX.open("GET","/process?code=INF&patch=" , true);
			_AJX.send(null); 
		
		}
		function update_file_content(){
			var patch= document.getElementById('_path').value ;
			var file= document.getElementById('_file').value;
			var content=(document.getElementById('_content').value); 
			console.log("/update_file_content?code=RUF&patch=" + patch+ file+ "&" + content);
			_Connection.send("RUF=" + patch+ file+ "&" + content);
			document.getElementById('_content').value="";}
		
		function removefile(){
			var path=document.getElementById("_path").value;
			var file=document.getElementById("_file").value;
			var _AJX=createXHR();    
			_AJX.onreadystatechange = function(){if(_AJX.readyState == 4){
																			var recieved =_AJX.responseText; show_list_file();}}
			console.log("/removefile?code=RRF&patch=" + path+ '&file='+file);
			_AJX.open("GET","/process?code=RRF&patch=" + path+ '&file='+file, true);
			_AJX.send(null);}
		
		function CreateFile() {
			var path=document.getElementById("_path").value;
			var file=document.getElementById("newFile").value;
			var _AJX=createXHR();    
			_AJX.onreadystatechange = function(){if(_AJX.readyState == 4){
																			var recieved =_AJX.responseText;  show_list_file();}}
			console.log("/CreateFile?code=CRF&patch=" + path+ '&file='+file);
			_AJX.open("GET","/process?code=CRF&patch=" + path+ '&file='+file, true);
			_AJX.send(null);
			}
		
		function renamefile(){
			var path=document.getElementById("_path").value;
			var oldfile=document.getElementById("_file").value;
			var newfile=document.getElementById("newFile").value;
			var _AJX=createXHR();    
			_AJX.onreadystatechange = function(){   
			if(_AJX.readyState == 4){  
			var recieved =_AJX.responseText;  show_list_file();}}
			console.log("/renamefile?code=RNF&patch=" + path+ '&oldfile='+oldfile + '&newfile='+newfile);
			_AJX.open("GET","/process?code=RNF&patch=" + path+ '&oldfile='+oldfile + '&newfile='+newfile, true);
			_AJX.send(null); }
		
		function set_Wifi_pass(){
			var net=document.getElementById("selectedWifi").value;
			var pass=document.getElementById("pass_box").value;
			var _AJX=createXHR();    
			_AJX.onreadystatechange = function(){if(_AJX.readyState == 4){}}
			console.log("/set_Wifi_pass?code=SWP&net=" + net+ '&pass='+pass);
			_AJX.open("GET","/process?code=SWP&net=" + net+ '&pass='+pass , true);
			_AJX.send(null);
		
		}
		function select_file_path(){
				var e = document.getElementById("listfile");
				var line = e.options[e.selectedIndex].text;
				var myArray=line.split("/");
				var num=myArray.length; 
				var li=line.lastIndexOf("/");
				//alert()//myArray+"  "+myArray.length+" "+myArray[num-1]);


				if(num>2){ 
				var qwe=line.substring(0,li);
				if(qwe.startsWith("/")) qwe=qwe.substring(1); 
				document.getElementById("_path").value='/'+qwe; 
				document.getElementById("_file").value=    myArray[num-1]; }
				else {
				document.getElementById("_path").value='/'; 
				document.getElementById("_file").value=myArray[0];}
				}
		function selectssid_pass(){ //<select name=\"Combobox2\" size=\"10\" id=\"_netList\"  onChange=\"selectssid_pass();\"
				var e = document.getElementById("_netList");
				if(e.selectedIndex>0)
				{	
				var strUser = e.options[e.selectedIndex].text;
				var S=strUser.split(",")
				var ssid=S[0].trim()
				document.getElementById('selectedWifi').value=ssid;
				}
			}
		
		function SetWifiMode(){				//WIM
				var e=document.getElementById("WifiMode").value;
				var _AJX=createXHR();    
			   _AJX.onreadystatechange = function(){ if(_AJX.readyState == 4) {   }  }
			   var command  = "?code=WIM&sf="+e;
			   console.log("/SetWifiMode?code=WIM&sf="+e);
			   _AJX.open("GET", "/process" + command, true);
			   _AJX.send(null); 
			}
		
		function restart(){			//RST
						  var _AJX=createXHR();    
						   _AJX.onreadystatechange = function(){ if(_AJX.readyState == 4) {  } }
						   var command  = "?code=RES";
						   console.log("/restart?code=RES");
						   _AJX.open("GET", "/process" + command, true);
						   _AJX.send(null); 
						}

		function upload_file(){
					var file=document.getElementById("FileUpload1");
					var path=document.getElementById("_path").value + document.getElementById("path").value;
					if(file.files.length === 0)  return;   
					var formData = new FormData();

					   formData.append("data", file.files[0], path); 
					var _AJX = new XMLHttpRequest(); 
					_AJX.onreadystatechange = function()
					{
					  if (_AJX.readyState == 4){ 
												if(_AJX.status != 200)    console.log("ERROR["+_AJX.status+"]: "+file+path+_AJX.responseText); 
												else                     show_list_file(); 
												Show_Image_List();
												}
					}; 
					console.log("/upload");
					_AJX.open("POST", "/upload");
					_AJX.send(formData); 
					}
		
		function check_validity() {
		 var file=document.getElementById("FileUpload1");
		 var path=document.getElementById("path");
		  if(file.files.length === 0)     return; 
		  var filename = file.files[0].name; 
		  var ext = /(?:\.([^.]+))?$/.exec(filename)[1]; 
		  var name = /(.*)\.[^.]+$/.exec(filename)[1]; 
		  if(typeof name !== undefined){ filename = name; } 
		  if(typeof ext !== undefined){ 
			   if(ext === "html") ext = "htm"; 
		  else if(ext === "jpeg") ext = "jpg"; 
		  filename = filename + "." + ext; }
		  if(path.value === "/" || path.value.lastIndexOf("/") === 0)  { path.value = "/"+filename; } 
		  else { path.value = path.value.substring(0, path.value.lastIndexOf("/")+1)+filename; }
		 }  
		 
		function ScanNetwork(){
								 var _AJX=createXHR();    
							   _AJX.onreadystatechange = function(){ if(_AJX.readyState == 4) {  
																		var rec= _AJX.responseText;	
																		console.log("<<<<<" + rec);
																		document.getElementById('netScanResult').innerHTML =rec	; 							
																	  } }
							  var command  = "?code=NET";  console.log("ScanNetwork/" + command);
							   _AJX.open("GET","/process" + command, true);
							   _AJX.send(null);
								
							}
	
	</script>
	<script type="text/javascript">      
				//window.onload = function (){setTimeout(Request_IP,1000);} 
	</script>
	<style>
		body {  background-color: #cbced1;}
		* {  box-sizing: border-box;}

		/* Create two equal columns that floats next to each other */
		.column {
		  float: left;
		  width: 50%;
		  padding: 2px;
		  height: 300px; /* Should be removed. Only for demonstration */
		}

		/* Clear floats after the columns */
		.row:after {
		  content: "";
		  display: table;
		  clear: both;
		}

		.Cleft {
		  width: 40%;
		}

		.Cright {
		  width: 60%;
		}

		table {
		  border-collapse: collapse;
		  border-spacing: 0;
		  width: 100%;
		  border: 0px solid #ddd;
		}

		th, td {
		  text-align: left;
		  padding-top: 5px;
		}

		 

		div.center {
		  text-align: center;
		}

		div.left {
		  text-align: left;
		}

		div.right {
		  text-align: right;
		} 

		div.justify {
		  text-align: justify;
		} 



		/* Responsive layout - makes the two columns stack on top of each other instead of next to each other */
		@media screen and (max-width: 600px) {
		  .column {
			width: 100%;
		  }
		}
		</style>
</head>
<body>
 <div class="center">
	<h2>Smart Machine</h2>
	<p style="color:blue;"> <b>OXIN</b> </p>
	<p>
	Device IP:<span id="thisIP" style="font-size:20px">---</span>	
	NetWork:<span   id="thisNet" style="font-size:20px">---</span>
	Mode:<span      id="thisMode" style="font-size:20px">--</span>
	</p>
	<input  id="reset" value="Reset" style="width:100px" onclick="restart();" type="button">
 </div>

<div class="row">

  <div class="column Cleft"  style="position: relative;">
	  
	  <input type="button" id="Button5" onclick="update_file_content();return false;" name="" value="Update File content" style="margin-bottom:5px;height:25px;">
	  <input type="button" id="ddButtosdn2" onclick="all_information();return false;" name="" value="Information"         style="margin-bottom:5px;height:25px;position:absolute;right:30%;">
	  <input type="button" id="Button2" onclick="show_file_content();return false;" name="" value="Show content"          style="margin-bottom:5px;height:25px;position:absolute;right:1px;">
	  
	  <textarea name="TextArea1" id="_content" style="width:100%;" rows="15"> </textarea>
		
		
    
    
  </div>
  <div class="column Cright" style="position: relative;">  
    
	<div  style="position:absolute;right:10px;" >
		<input type="text" value=""  id="path"   name="Editbox1"      style="width:100px;height:23px;">
		<input type="file" id="FileUpload1" name="FileUpload1"       style="width:98px;"  multiple="false" onchange="check_validity();return false;">
		<input type="button" value="Upload file"  id="Bu51on1" onclick="upload_file();return false;" name="" style="width:73px;">
	</div>
 
<br> <br> 

<div  >
  <table>
    <tr>
      <td><input type="button" id="Button10" onclick="ScanNetwork();return false;" name="" value="Search Wifi"  style="background-color: Gray;font-size: 18px;" ></td> <td></td>
      
      <td><input type="button" value="show file List" id="Button3" onclick="show_list_file();return false;" name=""  style="background-color: Gray;font-size: 18px;" ></td>
	  <td><input type="button" id="Button1" onclick="removefile();return false;" name="" value="Remove file"  ></td>
      
    </tr>
	<tr>
		<td>
			Wifi Mode   <select id="WifiMode" name="option" style="width:100px">
						<option value="0" selected="">AP Wifi</option>
						<option value="1">Conected Wifi</option>
			</select>	
		</td>
		<td>
			<button   onclick="SetWifiMode()" style="height:auto;width:60px">Set</button>
		</td>
		
		<td>
			<input type="button" id="Button6" onclick="CreateFile();return false;" name="" value="Create    file"  >
			<input type="button" id="Button4" onclick="renamefile();return false;" name="" value="Rename file"  >
		</td>
		<td>New File Name<input type="text" id="newFile" style="width:100px;"  name="Enbox1" value="" text-align="right"></td>
	</tr>
 
    <tr>
      <td>Selected Network</td>
      <td><input type="text"   id="selectedWifi" style="width:140px;"  name="Enbox1" value="" text-align="right" readonly ></td>
	  
      <td> Select File </td>
      <td> </td>
   
    </tr>
    <tr>
      <td><input type="button" id="Buaweon10" onclick="set_Wifi_pass();return false;" name="" value="Set password"    ></td>
      <td>Pass <input type="text"   id="pass_box" style="width:100px;"  name="Enbox1" value="" text-align="right"></td>
	  
      <td>Path<input type="text" id="_path"   style="width:100px; "  name="Editbox1" value="/" text-align="right"></td>
      <td>File<input type="text" id="_file"   style="width:90px;"    name="Editbox1" value="" text-align="right"></td>
    
    </tr>
    <tr>
 
	 <td><div id="netScanResult"  style="position:absolute;left:10px;height:200px;"  ></div></td>
	  <!-- 
	  <select name="Combobox2" size="10" id="_netList"  onChange="selectssid_pass();" 
	  -->
     <td><div id="showfiles"    style="position:absolute;right:200px;height:200px;"></div></td>
     <!--
	 <select name="Combobox1" size="10" id="listfile"  onChange="select_file_path();"   >
	 -->
    </tr>
  </table>
</div>




 </div>
 
</div>


 
</body>
</html>


 

)rawliteral";

#endif