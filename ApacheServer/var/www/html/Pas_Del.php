<html>
<head>
	<title> TEST </title>
</head>
<body>
	<?php
			@mysql_connect("localhost", "root", "root") or
			    die("Could not connect: " . mysql_error());
			mysql_select_db("tech_schem");

			$q = $_GET["_id"];
			$result = mysql_query("SELECT pid from passengers WHERE ". $q . ";");			
			$sen_id = mysql_result($result,0,"pid");
			
			if($sen_id!=$q) 
			{		
				echo ("\"".$q."\""." Delete Fail!<br>Not in DB<br>");				
			}
			else
			{
				mysql_query("DELETE FROM passengers where pid = '". $q . "';");
				echo ("\"".$q."\""." Delete Clear!<br>");
			}
			mysql_free_result($result); 			
			
			echo ("<br>Moving to list page...");
			echo ("<meta http-equiv='refresh' content='2;url=./listup.php'>");
		?>
</body>
</html>
