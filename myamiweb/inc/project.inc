<?php

/**
 *	The Leginon software is Copyright under 
 *	Apache License, Version 2.0
 *	For terms of the license agreement
 *	see  http://leginon.org
 */

if (ENABLE_LOGIN) {
	require_once "inc/dbemauth.inc";
}

require_once "inc/session.inc";
require_once "inc/leginon.inc";

define('DEFAULT_PROJECT', 'all'); 
define('PROJECT_DB_HOST', $PROJECT_DB_HOST);
define('PROJECT_DB_USER', $PROJECT_DB_USER);
define('PROJECT_DB_PASS', $PROJECT_DB_PASS);
define('PROJECT_DB', $PROJECT_DB);

setsession();

function setDatabase($expId="") {
	global $leginondata;
	if (!$expId) {
		$expId= ($_POST['sessionId']) ? $_POST['sessionId'] : $_GET['expId'];
	}
	$projectId = ($_POST['projectId']) ? $_POST['projectId'] : $_GET['projectId'];
	if ($projectId) {
		$sessions = $leginondata->getSessions('description', $projectId);
		$sessionId_exists = $leginondata->sessionIdExists($sessions, $expId);
		if (!$sessionId_exists) {
			$expId=$sessions[0]['id'];
		}
	}
	if (!$expId) {
		if ($_GET['session']) {
			$expId=$_GET['session'];
		} else {
			$expId=$leginondata->getLastSessionId();
		}
	}
	$p=new project();
	$r=$p->getProjectfromSessionId($expId);
	$projectId=$r['projectId'];
	$db=$p->getProcessingDB($projectId);
	$_SESSION['processingdb']=$db;
	$_SESSION['expId']=$expId;
	return $db;
}

if (!$PROCESSING_DB) {
	$PROCESSING_DB=setDatabase($newexpId);
}

define('PARTICLE_DB', $PROCESSING_DB);

class project {

	function project(){
		$this->mysql = new mysql(	DB_HOST, DB_USER,
			DB_PASS, DB_PROJECT);
	}

	function checkDBConnection() {
		return $this->mysql->checkDBConnection();
	}

	function getGridInfo($gridId){
		// deprecated
		return false;
		$q='select g.gridId, label as "label", '
			.'date_format(g.prepdate,"%m/%e/%Y") as prepdate, '
			.'g.specimenId, '
			.'g.substrate, '
			.'g.preparation, '
			.'g.number, '
			.'g.note, '
			.'g.boxId, '
			.'g.peopleId, '
			.'l.location '
			.'from grids g '
			.'left join gridlocations l '
			.'on (l.gridId = g.gridId) '
			.'where g.gridId="'.$gridId.'"';

		list($info) = $this->mysql->getSQLResult($q);
		return $info;
	}

	function getProjectFromSession($session) {
		$q='select p.`DEF_id` as projectId, p.name  '
			.'from projectexperiments pe '
			.'left join projects p '
			.'on (pe.`REF|projects|project` = p.`DEF_id`) '
			.'left join '.DB_LEGINON.'.`SessionData` s '
			.'on (s.`DEF_id` = pe.`REF|leginondata|SessionData|session`) '
			.'where s.name="'.$session.'"';
		list($info) = $this->mysql->getSQLResult($q);

		$projectId=$info['projectId'];

		return $info;
	}

	function sessionExists($projectId, $sessionId) {
		if (!is_numeric($projectId)) {
			return true;
		}
		$projectinfo = $this->getProjectfromSessionId($sessionId);
		$nprojectId = $projectinfo['projectId'];
		return ($nprojectId==$projectId) ? true : false;
	}

	function getProjectfromSessionId($sessionId) {
 		$q="SELECT projexp.`REF|projects|project` AS projectId  "
			."FROM projectexperiments AS projexp "
			."WHERE projexp.`REF|leginondata|SessionData|session` = '$sessionId' ";
		list($info) = $this->mysql->getSQLResult($q);

		$projectId=$info['projectId'];
		
		return $info;
	}

	function getProjects($type=""){
		
		$userId = getLoginUserId();
		$privilege_level = privilege('projects');
		global $dbemauth;
		
		// If the users privilege level is admin(4) or power user(3),
		// they may view all projects, otherwise they may only 
		// view projects they own or projects that contain experiments
		// that have been shared with the user.
		$findOwnedProjects = ($privilege_level <= 2 && $userId); 

		if ($findOwnedProjects) {		
			$sharedProjectsQuery = " SELECT ".DB_PROJECT.".projectexperiments.`REF|projects|project` "
				." FROM ".DB_PROJECT.".projectexperiments " 
				." WHERE ".DB_PROJECT.".projectexperiments.`REF|leginondata|SessionData|session` IN ( "
						." SELECT ".DB_PROJECT.".shareexperiments.`REF|leginondata|SessionData|experiment` "
						." FROM ".DB_PROJECT.".shareexperiments "
						." WHERE ".DB_PROJECT.".shareexperiments.`REF|leginondata|UserData|user` = ".$userId." ) ";

			// Build a query to find the projects that are owned by the user, then add on the query to find
			// the projects that are shared with the user.
			$ownedProjectsQuery = "SELECT projectowners.`REF|projects|project` "
				."FROM projectowners WHERE projectowners.`REF|leginondata|UserData|user` = ".$userId." "; 
		$q = "SELECT projects.`DEF_id` as id "
				."FROM ".DB_PROJECT.".projects as projects "
				."WHERE projects.`DEF_id` IN ($sharedProjectsQuery) "
				."OR projects.`DEF_id` IN ($ownedProjectsQuery) "
				."GROUP BY `DEF_id` "
				."ORDER by projects.`name` ";
		$myprojects = $this->mysql->getSQLResult($q);
		}
		
		// The hidden field of the projects table apparently may not exist. 
		// Don't know the history of that.
		// If it does exist, we do not want to show hidden projects
		$projectdef = $this->mysql->getFieldTypes('projects');
		
		// SQL query to select the projects with experiment sessions assigned
		$q = "SELECT projexp.`REF|projects|project` AS id, "
			."   projects.`name` AS name, "
			."   DATEDIFF(NOW(), MAX(projexp.`DEF_timestamp`)) AS age "
			." FROM projectexperiments AS projexp "
			." LEFT JOIN projects "
			."   ON projects.`DEF_id` = projexp.`REF|projects|project` "
			." WHERE projexp.`REF|projects|project` IS NOT NULL AND projects.`name` IS NOT NULL ";

		$q .= " GROUP BY projexp.`REF|projects|project` ";
		// Make sure the projects appear in alphabetical order
		$q .= ' ORDER by projects.`name`';
		$rprojects = $this->mysql->getSQLResult($q);

		// Find from the results the projects owned by the user or have shared experiment
		// This is done separately to speed up the query
		if ($findOwnedProjects) {
			$projectsbyid = array();
			foreach ($rprojects as $project_w_experiments)
				$projectsbyid[$project_w_experiments['id']] = $project_w_experiments;
			$rprojects = array();
			foreach ($myprojects as $project)
				if (in_array($project['id'],array_keys($projectsbyid)))
					$rprojects[] = $projectsbyid[$project['id']];
		}
		#echo $q."<br/><br/>\n";

		$projects = array();
		if ($type=="all") {
			$projects[] = array(	'id' => DEFAULT_PROJECT, 'name' => DEFAULT_PROJECT);
		}
		$projects = array_merge($projects, $rprojects);
		return $projects;
	}

	function getSample($sessioninfo="") {
		# get sample name from project database
		if (SAMPLE_TRACK) {
			$sessionId = ($sessioninfo['SessionId'])? $sessioninfo['SessionId'] : $sessioninfo['Id'];
			list($proj, $packagenb, $samplenb) = explode(".", $sessioninfo['Purpose']);
			$packagenb = (int) preg_replace("%P%", "", $packagenb);
			$samplenb = (int) preg_replace("%S%", "", $samplenb);
			/* if package number is 0, then don't join package table in this query */
			if ($packagenb == 0) {
				$q='select s.packageId, s.`number`, s.label from '.DB_PROJECT.'.nsample s
					left join '.DB_PROJECT.'.projects pr on (pr.`DEF_id`=s.projectId)
					left join '.DB_PROJECT.'.projectexperiments pe on (pe.`REF|projects|project`=pr.`DEF_id`)
					where
					pe.`REF|leginondata|SessionData|session`='.$sessionId.'
					and s.`number`="'.$samplenb.'"
					and s.packageId IS NULL
					';
			} else {
				$q='select s.packageId, p.`number` as packageNumber, s.`number` as sampleNumber, s.label from '.DB_PROJECT.'.nsample s
					left join '.DB_PROJECT.'.npackage p on (p.packageId=s.packageId)
					left join '.DB_PROJECT.'.projects pr on (pr.`DEF_id`=s.projectId)
					left join '.DB_PROJECT.'.projectexperiments pe on (pe.`REF|projects|project`=pr.`DEF_id`)
					where
					pe.`REF|leginondata|SessionData|session`='.$sessionId.'
					and p.`number`="'.$packagenb.'"
					and s.`number`="'.$samplenb.'"
					';
			}
			list($res)=$this->mysql->getSQLResult($q);
			$tag="";
			if ($res['label']) {
				$tag=$res['label'];
			}
			return $tag;
		} else {
			return "";
		}
	}

	function getProjectInfo($projectId) {
		$q='select DEF_id AS projectId, name from projects '
			.'where DEF_id='.$projectId;
		list($r)=$this->mysql->getSQLResult($q);

		return $r;
	}

	function getSessions($sessions) {		
		return $sessions;		
	}

	function hasProjectId($projectId, $projects) {
		$exists=false;
		foreach ((array)$projects as $p) {
			if ($p['projectId']==$projectId) {
				$exists=true;
				break;
			}
		}
		return $exists;
	}

	function getProcessingDB($projectId) {
		$db="";
		$q="select `appiondb` from processingdb "
			."where `REF|projects|project`=$projectId";
		if ($r=$this->mysql->getSQLResult($q)) {
			$db=$r[0]['appiondb'];
		}
		return $db;
	}

}
?>
