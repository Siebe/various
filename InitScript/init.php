<?php

/****************************************
*   _______ _________________ __ ____   *
*  //  || \\|  // \\ || ||||| ||||      *
* ((   ||_//|==||=|| ||  ||\\ //||==    *
*  \\__|| \\|__|| || ||  || \V/ ||___   *
*     _______  ___ __    ______ __      *
*    //  || \\// \\||    |||| \(( \     *
*   ((   ||_/((   ))\ /\ //||  ))\      *
*    \\__|| \\\\_// \V/\V/ ||_/|_))     *
*                                       *
*  ######## CreativeCrowds ##########   *
*                                       *
* Keizersgracht 119                     *
* 1015 CJ Amsterdam                     *
* info@creativecrowds.nl                *
* http:://creativecrowds.nl             *
*                                       *
*****************************************/


/***********************************************
* Creative Crowds Server Initialization Script *
* By ~Siebe                                    *
*                                              *
* This script can be used to initialize one of *
* the Creative Crowds projects such as         *
* 'Verbeterdebuurt'. It requires a config file *
* written in YAML, see the example             *
* 'config.yml'.                                *
***********************************************/

/*
 * TODO: 
 * - Test test test
 * - Do some more with symfony tests (colors at least)
 * - Apache modules in prequisites
 * optional:
 * - etc/hosts, might be handy for testing, even without apache but we don't generally set this on servers
 * - configurable prequisites/dependencies
 * - dry-run mode would be nice, but kinda difficult ;)
 * - Replace the evil eval 
 * - More text formatting
 */

/*
 * Return codes:
 * 0 - Normal termination
 * 1 - Missing prequisites
 * 2 - Config file error
 * 3 - Filesystem write/read error
 * 4 - MySQL error
 */

//includes
set_include_path('includes'); //main include path
include('Net/SFTP.php'); //SFTP functions
include('Spyc.php'); //Yaml functions
//sftp logging
define('NET_SFTP_LOGGING', NET_SFTP_LOG_COMPLEX);


// <editor-fold defaultstate="collapsed" desc="option_handling">
/*******************
 * Option Handling *
 ******************/

/* Short options list:
 * -f(filename) : Use different config file 
 * -h : Display help and exit
 * -y : Ignore prompt (yes on all)
 * -n : Ignore prompt (no on all, safe mode)
 * -s : Silent mode (also invokes -i if -x is not given)
 * -a : Do NOT restart apache
 * -c : Force copy
 */

$options = getopt("f::ynshac", array(
    "skip-dir",
    "skip-git",
    "skip-file",
    "skip-link",
    "skip-yml",
    "skip-sql",
    "skip-apache",
    "skip-symfony",
    "help"
));
//help option
if (isset($options['h']) || isset($options['help'])) {
    echo "
    ------------------------------------------------
    | Creative Crowds Server Initialization Script |
    | By Siebe                                     |
    |----------------------------------------------|
    
    This script can be used to initialize one of 
    the Creative Crowds projects such as 
    'Verbeterdebuurt'. It requires a config file
    written in YAML, see the example 'config.yml'.
    This script does the following things:
    - Prequisites check (dependencies)
    - Load config (this script's config YAML file)
    - Create new directories
    - Git init and pull
    - File replacement (download non-git files)
    - Create symlinks
    - YAML replacement (change project settings)
    - Prepare database (SQL)
    - Cronjobs
    - Config Apache
    - Symfony cache and test
    
Options:
    -f(filename)        Specify a different config YAML file
    -h, --help          Display this help and exit
    -y                  Ignore prompt (yes on all)
    -n                  Ignore prompt (no on all, safer)
    -s                  Silent mode (Ignore prompt, do yes by default)
    -a                  Do not restart apache after file/db transfer
    -c                  Force copy (if file/directory exists)
Skip Options:
    --skip-dir          Skip create new directories
    --skip-git          Skip git init and pull
    --skip-file         Skip file replacements
    --skip-link         Skip create symlinks
    --skip-yml          Skip YAML replacements
    --skip-sql          Skip prepare database
    --skip-apache       Skip config (and restart!) Apache
    --skip-cronjobs     Skip cronjobs
    --skip-symfony      Skip Symfony cache and test
    
Example:
Run the script with otherconfig.yml in silent mode. Will exit 
automatically if there is any warning/problem. Also option -a
is set, because the apache restart will prompt for a password 
even when in silent mode. And finally we'll skip the symfony
cache and test, as that is a bit useless in silent mode:
    
    php init.php -fotherconfig.yml -nsa --skip-symfony
    
";
return 0;
}  
//other options
$config_file = "config.yml";
$silent = false;
$ytoall = false;
$ntoall = false;
$apache_no_restart = false;
$force_copy = false;
//ytoall takes precedence over ntoall, if silent is defined without ntoall or ytoall, ytoall will be invoked
if (isset($options['y'])) $ytoall = true;
if (isset($options['n']) && !$ytoall) $ntoall = true;
if (isset($options['s'])) {
    $silent = true;
    if (!$ytoall && !$ntoall) {
        $ytoall = true;
    }
}
if (isset($options['a'])) $apache_no_restart = true;
if (isset($options['c'])) $force_copy = true;
//config file option, must have yaml or yml extension
if (isset($options['f'])) {
    if (!file_exists($options['f'])) {
        if (!$silent) echo "Specified config file not found!";
        return 2;
    }
    if (!preg_match("/\.ya?ml$/", $options['f'])) {
        if (!$silent) echo "Warning: specified config file is not a YAML file!";
        if (!$user_continue) return 0;
    }
    $config_file = $options['f'];
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="prequisites_check">
/*********************
 * Prequisites check *
 ********************/

//check php version >= 5.3
if (!defined('PHP_VERSION_ID') || PHP_VERSION >= 50300) {
    if (!$silent) echo "PHP version 5.3 or higher required.\n";
    return 1; // return code 1 - missing prequisites
}

//return variables for exec command
$cli_ret = array();
$code_ret;

//check apache version >= 2.2
$apache_use_httpd = false;
exec("apache2 -v", $cli_ret, $code_ret); 
if ($code_ret > 0) {
    //if the apache2 command does not return 0, do try the older httpd command once 
    exec("httpd -v", $cli_ret, $code_ret);
    $apache_use_httpd = true;
} 
if ($code_ret > 0) {
    if (!$silent) echo "Could not execute apache2 or httpd, Apache seems not installed.\n";
    return 1;
}
$apache_v = split("\.", preg_replace("/.*(\d+\.\d+(\.\d+)?).*/U", "$1", $cli_ret[0]));
if(sizeof($apache_v) < 2 || $apache_v[0] < 2 || ($apache_v[0] == 2 && $apache_v[1] < 2)) {
    if (!$silent) echo "Apache version 2.2 or higher required.\n";
    return 1;
}

//get apache root dir
$cli_ret = array();
if ($apache_use_httpd) {
    exec("httpd -V", $cli_ret, $code_ret);
} else {
    exec("apache2 -V", $cli_ret, $code_ret);
}
$apache_root = false;
foreach ($cli_ret as $line) {
    if (preg_match("/^ -D HTTPD_ROOT=/", $line)) {
        $apache_root = preg_replace("/^ -D HTTPD_ROOT=\"(.*)\"$/", "$1", $line)."/";
        break;
    }
}

if (!$apache_root) {
     if (!$silent) echo "Apache HTTPD_ROOT directory not found, is apache installed correctly?\n";
    return 1;   
}
//check git version >= 1.7
$cli_ret = array();
exec("git --version", $cli_ret, $code_ret); 
if ($code_ret > 0) {
    if (!$silent) echo "Could not execute git, Git seems not installed.\n";
    return 1;
}
$git_v = split("\.", preg_replace("/.*(\d+\.\d+(\.\d+){0,2}?).*/U", "$1", $cli_ret[0]));
if(sizeof($git_v) < 2 || $git_v[0] < 1 || ($git_v[0] == 1 && $git_v[1] < 7)) {
    if (!$silent) echo "Git version 1.7 or higher required.\n";
    return 1;
}

//check mysql version >= 5.5
$cli_ret = array();
exec("mysql --version", $cli_ret, $code_ret); 
if ($code_ret > 0) {
    if (!$silent) echo "Could not execute mysql, mysql seems not installed.\n";
    return 1;
}
$mysql_v = split("\.", preg_replace("/.* Distrib (\d+\.\d+(\.\d+)?).*/U", "$1", $cli_ret[0]));
if(sizeof($mysql_v) < 2 || $mysql_v[0] < 5 || ($mysql_v[0] == 5 && $mysql_v[1] < 5)) {
    if (!$silent) echo "MySQL version 5.5 or higher required.\n";
    return 1;
}
                    
if (!$silent) echo "Prequisites check....................................................OK\n\n";
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="load_config">
/***************
 * Load Config *
 **************/

//Load config file, check main contents
$config = Spyc::YAMLLoad($config_file);
if (sizeof($config) < 2) {
    if (!$silent) echo "Error while loading $config_file, file seems to be empty or unfinished.\n";
    return 2; //return code 2 - config file error
}


//Load project information from config file
if (!isset($config['project'])) {
    if (!$silent) echo "Missing project information in $config_file.\n";
    return 2;
}
if (!isset($config['project']['name'])) {
    if (!$silent) echo "Missing project name in $config_file.\n";
    return 2;
}
$project_name = $config['project']['name'];
if (!isset($config['project']['url'])) {
    $project_url = "http://www.".strtolower($project_name).".nl";
} else {
    $project_url = $config['project']['url'];
}
if (!isset($config['project']['maindir'])) {
    $project_maindir = "/www/www.".strtolower($project_name).".nl/";
} else {
    $project_maindir = $config['project']['maindir'];
}
if (!isset($config['project']['gitdir'])) {
    $project_gitdir = $project_maindir;
} else {
    $project_gitdir = $config['project']['gitdir'];
}
if (!isset($config['project']['symfonydir'])) {
    $project_symfonydir = $project_maindir;
} else {
    $project_symfonydir = $config['project']['symfonydir'];
}
if (!isset($config['project']['webroot'])) {
    $project_webroot = $project_symfonydir."web/";
} else {
    $project_webroot = $config['project']['webroot'];
}
if (!isset($config['project']['logdir'])) {
    $project_logdir = $project_symfonydir."log/";
} else {
    $project_logdir = $config['project']['logdir'];
}


//Load database information from config file
if (!isset($options['skip-sql'])) {
    if (!isset($config['database'])) {
        if (!$silent) echo "Missing database information in $config_file.\n";
        return 2;
    }
    if (!isset($config['database']['root_user'])) {
        $db_root_user = "root";
    }
    $db_root_user = $config['database']['root_user'];
    if (!isset($config['database']['root_pw'])) {

        if (!$silent)  {
            echo "Missing password for root user '$db_root_user', needed for database access\n";
            echo "Please supply password (leave empty to exit): ";
            $input = preg_replace('/\r?\n$/', '', `stty -echo; head -n1 ; stty echo`);
            echo "\n";
            if ($input != "") {
                $db_root_pw = $input;
            } else return 0;
        } else return 2;
    } else {
        $db_root_pw = $config['database']['root_pw'];
    }
    if (!isset($config['database']['host'])) {
        $db_host = "localhost";
    } else {
        $db_host = $config['database']['host'];
    }
    if (!isset($config['database']['name'])) {
        $db_name = strtolower($project_name);
    } else {
        $db_name = $config['database']['name'];
    }
    if (!isset($config['database']['user'])) {
        if (!$silent) echo "Missing database username in $config_file.\n";
        return 2;
    }
    $db_user = $config['database']['user'];
    if (!isset($config['database']['password'])) {
        //generate random password for db user
        $db_password = substr(md5(uniqid()), 0, 8);
        if (!$silent) {
            echo "Missing password for database user '$db_user', this script should create that user\n";
            echo "Please supply password (leave empty for random): ";
            $input = preg_replace('/\r?\n$/', '', `stty -echo; head -n1 ; stty echo`);
            echo "\n";
            if ($input != "") {
                $db_password = $input;
            }
        }

    } else {
        $db_password = $config['database']['password'];
    }
    if (!isset($config['database']['fixture'])) {
        $db_fixture = strtolower($db_name).".sql";
    } else {
        $db_fixture = $config['database']['fixture'];
    }
}

//Load git information from config file
if (!isset($options['skip-git'])) {
    if (!isset($config['git'])) {
        if (!$silent) echo "Missing Git information in $config_file.\n";
        return 2;
    }
    if (!isset($config['git']['repo'])) {
        $git_repo = "https://github.com/sbalen/".ucfirst(strtolower($project_name)).".git";
    } else {
        $git_repo = $config['git']['repo'];
    }
    if (!isset($config['git']['branch'])) {
        $git_branch = "master";
    } else {
        $git_branch = $config['git']['branch'];
    }
    if (!isset($config['git']['user'])) {
        if (!$silent) echo "Missing git username in $config_file.\n";
        return 2;
    } else {
        $git_user = $config['git']['user'];
    }
    if (!isset($config['git']['password'])) {
        $git_password = false;
        if (!$silent) {
            echo "Missing password for Git user '$git_user', might not need pw\n";
            echo "Please supply password (leave empty for no pw or pw in Git config): ";
            $input = preg_replace('/\r?\n$/', '', `stty -echo; head -n1 ; stty echo`);
            echo "\n";
            if ($input != "") {
                $git_password = $input;
            }
        }
    } else {
        $git_password = $config['git']['password'];
    }
}

#load file replacements if any
$file_replacements = array();
if (!isset($options['skip-files'])) {  
    $fail = false;
    if (isset($config['files'])) {
        if (!is_array($config['files'])) {
            if (!$silent) echo "Warning: section 'files' in $config_file is not an YAML array!\n";
            $config['files'] = null;
            $fail = true;
        }
        foreach ($config['files'] as $file) {
            if (!isset($file['source']) || !isset($file['target'])) {
                if (!$silent) echo "Warning: missing source or target in file replacement list (files) in $config_file.\n";
            } else {
                if (strrpos($file['source'], ":") !== false && (!isset($file['ssh_user']))) {
                    if (!$silent) echo "Warning: source '".$file['source']."' seems an external location, but missing ssh user in $config_file.\n";
                    $fail = true;
                } else {
                    $file_replacements[] = $file;
                }
            }
        }
    }
}

#load symlinks to create
$symlinks = array();
if (!isset($options['skip-symlinks'])) {   
    if (isset($config['symlinks'])) { 
        if (!is_array($config['symlinks'])) {
            if (!$silent) echo "Warning: section 'symlinks' in $config_file is not an YAML array!\n";
            $config['yaml_replacements'] = null;
            $fail = true;
        }
        foreach($config['symlinks'] as $link) {
            if (!isset($link['source']) || !isset($link['target'])) {
                if (!$silent) echo "Warning: missing source or target in symlinks list in $config_file\n";
                $fail = true;
            } else {
                $symlinks[] = $link;        
            }
        }
    }
}

#load YAML replacements if any
$yaml_replacements = array();
if (!isset($options['skip-yaml'])) {
    if (isset($config['yaml_replacements'])) { 
        if (!is_array($config['yaml_replacements'])) {
            if (!$silent) echo "Warning: section 'yaml_replacements' in $config_file is not an YAML array!\n";
            $config['yaml_replacements'] = null;
            $fail = true;
        }
        foreach($config['yaml_replacements'] as $yaml_rep) {
            if (!isset($yaml_rep['file']) || !isset($yaml_rep['key']) || !isset($yaml_rep['value'])) {
                if (!$silent) echo "Warning: each entry in yaml_replacements needs a file, key and value in $config_file\n";
                $fail = true;
            } else {
                $yaml_replacements[] = $yaml_rep;        
            }
        }
    }
}

#load Cronjobs if any
$cronjobs = array();
if (!isset($options['skip-cronjobs'])) {
    if (isset($config['cronjobs'])) { 
        if (!is_array($config['cronjobs'])) {
            if (!$silent) echo "Warning: section 'cronjobs' in $config_file is not an YAML array!\n";
            $config['conjobs'] = null;
            $fail = true;
        }
        foreach($config['cronjobs'] as $job) {
            if (!isset($job['timing']) || !(isset($job['php_file']) || isset($job['task']))) {
                if (!$silent) echo "Warning: each entry in yaml_replacements needs a timing and a php_file or task in $config_file\n";
                $fail = true;
            } else {
                $cronjobs[] = $job;        
            }
        }
    }
}
                        
if (!$silent && !$fail) echo "Load config..........................................................OK\n\n";
if (!$silent && $fail) echo "Load config................................Not OK (skipped some things)\n\n";


// </editor-fold>

//Clean part, remove all previous installed stuff we found in config
//Mostly function 'clean' with all options on, except crontab might use something more subtle
if (isset($options['--clean'])) {
    if (!$silent) "Warning: you are about to remove the project $project_name\n";
}

// <editor-fold defaultstate="collapsed" desc="create_directories">
/**********************
 * Create Directories *
 *********************/

if (!isset($options['skip-dir'])) {
    //check for existing directory, give warning if needed and remember to do/do not remove maindir if install fails
    $remove_maindir = true;
    if (is_dir($project_maindir)) {
        $remove_maindir = false;
        if (!$silent) echo "Warning: main directory already exists: $project_maindir\n";
        if (!user_continue()) {
            cleanup();
            return 0;
        }
    }

    //try and make needed directories
    if (
        (!@mkdir($project_maindir, 0755) && !(is_dir($project_maindir) && is_writable($project_maindir))) || 
        (!@mkdir($project_gitdir, 0755) && !(is_dir($project_gitdir) && is_writable($project_gitdir))) || 
        (!@mkdir($project_symfonydir, 0755) && !(is_dir($project_symfonydir) && is_writable($project_symfonydir))) || 
        (!@mkdir($project_logdir, 0777) && !(is_dir($project_logdir) && is_writable($project_logdir))) ||
        (!@mkdir($project_symfonydir."cache", 0777) && !(is_dir($project_symfonydir."cache") && is_writable($project_symfonydir."cache"))) ||
        (!@mkdir($project_webroot, 0755) && !(is_dir($project_webroot) && is_writable($project_webroot))) ||
        (!@mkdir($project_webroot."uploads", 0777) && 
                !(is_dir($project_webroot."uploads") && is_writable($project_webroot."uploads"))) || 
        (!@mkdir($project_webroot."uploads/issues", 0777) && 
                !(is_dir($project_webroot."uploads/issues") && is_writable($project_webroot."uploads/issues"))) ||
        (!@mkdir($project_webroot."uploads/thumbnails", 0777) && 
                !(is_dir($project_webroot."uploads/thumbnails") && is_writable($project_webroot."uploads/thumbnails")))
    ){
        if (!$silent) echo "Unable to create/write directory : $project_maindir\n";
        cleanup();
        return 3; //return code 3 - unable to write to disk
    }
                       
    if (!$silent) echo "Create directories...................................................OK\n\n";
} elseif (!$silent) echo "Create directories..............................................Skipped\n\n";
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="git_init_and_pull">

/**********************
 * Git init and pull  *
 *********************/

if (!isset($options['skip-git'])) {
    //Check for empty git in maindir
    $cli_ret = array();
    if (file_exists($project_gitdir.".git")) {
        exec("git --git-dir=\"$project_gitdir.git\" --work-tree=\"$project_gitdir\" status", $cli_ret, $code_ret); 
        if ($code_ret > 0) {
            if (!$silent) echo "Could not execute git in $project_gitdir.\n";
            if (!$silent) echo "Make sure git is properly installed!\n";
            cleanup();
            return 1;
        }
        if (!preg_match("/^fatal: Not a git repository/",$cli_ret[0])) {
            //existing repository found on this location, get some more info
            $cur_branch = preg_replace("/^# On branch (.*)\s*$/", "$1", $cli_ret[0]);
            if (!$silent) echo "Warning: git seems already initialized in $project_gitdir.\n";
            if (!$cur_branch) {
            if (!$silent) echo "Initialization will overwrite directory with unkown branch.\n";
            } else {
            if (!$silent) echo "Initialization will overwrite directory with current branch: $cur_branch.\n";
            }
            if (!$silent && isset($cli_ret[1]) && !preg_match("/^nothing to commit/", $cli_ret[1])) {
                echo "*Double Warning*: There might still be uncomitted changes on that branch!\n";
            }
            $cli_ret = array();
            //info about the existing branch
            exec("git --git-dir=\"$project_gitdir.git\" remote -v", $cli_ret, $code_ret);
            if (sizeof($cli_ret) > 0) {
                if (!$silent) {
                    echo "The branch likely belongs to one of the following remotes:\n";
                    foreach ($cli_ret as $remote) {
                        echo $remote."\n";
                    }
                }
            }    
            if (!$silent) echo "It's highly recommended to take another look at the current git status yourself,\n";
            if (!$silent) echo "as everything will be overwritten when continued!\n";
            if (!user_continue()) {
                cleanup();
                return 0;
            }
            //very well, remove the whole bunch, recreate log cache, web dirs
            exec("rm -rf \"$project_gitdir.git\"");
            exec("rm -rf \"$project_gitdir\"*");
            exec("rm -f \"$project_gitdir.gitignore\"");
            @mkdir($project_logdir, 0777);
            @mkdir($project_symfonydir."cache", 0777);
            @mkdir($project_symfonydir."web", 0755);
            @mkdir($project_symfonydir."web/uploads", 0777);
            @mkdir($project_symfonydir."web/uploads/issues", 0777);
            @mkdir($project_symfonydir."web/uploads/thumbnails", 0777);
        }
    }
    $git_clean = true;

    //build git remote address if password and user are present
    if ($git_password) {
        $git_temp_remote = preg_replace("/^https:\/\//", "https://$git_user:$git_password@", $git_repo);
    } else if ($git_user) {
        $git_temp_remote = preg_replace("/^https:\/\//", "https://$git_user@", $git_repo);
    } else {
        $git_temp_remote = $git_repo;
    }
    //Initialize!
    if (!@mkdir($project_gitdir.".git", 0775)) {
        if (!$silent) echo "Unable to create/write directory : $project_gitdir.git\n";
        cleanup();
        return 3; //return code 3 - unable to write to disk    
    }
    $cli_ret = array();
    exec("git --git-dir=\"$project_gitdir.git\" --work-tree=\"$project_gitdir\" init", $cli_ret, $code_ret);
    if ($code_ret != 0) {
        if (!$silent) echo "Unable to initialize git in $project_gitdir\n";
        cleanup();
        return 3;
    }
    //use the credential helper if no password is provided, so it will only prompt for it once
    if (!$git_password) {
        exec("git config --file \"$project_gitdir.git/config\" credential.helper cache");
    }
    //add the origin remote, we'll have to cd to the dir for this appearantly
    $cli_ret = array();
    exec("cd \"$project_gitdir\" && git remote add origin ".$git_repo, $cli_ret, $code_ret);
    if ($code_ret != 0) {
        if (!$silent) echo "Warning: unable to add remote host to git in $project_gitdir\n";
        if (!user_continue()) {
            cleanup();
            return 0;
        }
    }
    //Switch to (new) branch
    if ($git_branch != 'master') {
        $cli_ret = array();
        exec("git --git-dir=\"$project_gitdir.git\" --work-tree=\"$project_gitdir\" checkout -b $git_branch", $cli_ret, $code_ret);
        if ($code_ret > 0 && !$silent) echo "Warning: unable to create local branch $git_branch, staying in 'master'\n";
    }
    //And pull!
    if (!$silent) echo "Pulling from Git...\n";
    $cli_ret = array();
    if (!$silent || !$git_password) {
        exec("git --git-dir=\"$project_gitdir.git\" --work-tree=\"$project_gitdir\" pull $git_temp_remote ".$git_branch, $cli_ret, $code_ret);
    } else {
        exec("git --git-dir=\"$project_gitdir.git\" --work-tree=\"$project_gitdir\" pull $git_temp_remote $git_branch 2>/dev/null", $cli_ret, $code_ret);
    }
    if ($code_ret != 0) {
        if (!$silent) echo "Unable to pull git in $project_gitdir\n";
        cleanup();
        return 3;
    }
                       
    if (!$silent) echo "Git init and pull....................................................OK\n\n";
                         
} elseif (!$silent) echo "Git init and pull...............................................Skipped\n\n";
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="file_replacement">
/*********************
 * File replacement  *
 ********************/

if (!isset($options['skip-file'])) {
    #iterate al file_replacements, either exec cp command or connect with ssh
    if (sizeof($file_replacements > 1)) {
        $fail = false;
        foreach ($file_replacements as $file) {
            $cli_ret = array();
            #make target location absolute if it looks relative
            if (strpos($file['target'], PATH_SEPARATOR) !== 0) {
                $file['target'] = $project_maindir.$file['target'];
            }
            #check wether replacement is local or trhough sftp
            if (strpos($file['source'], ":") !== false) {
                #sftp copy, check for user
                if (!isset($file['ssh_user'])) {
                    if (!$silent) echo "Warning: missing user for ssh connection with ".$file['source']." - skipping file \n";
                    $fail = true;
                    continue;
                }
                #check for pw and prompt if needed
                if (!isset($file['ssh_pw'])) {
                    if (!$silent)  {
                        echo "Missing password for ssh user '".$file['ssh_user']."' for ".$file['source']."\n";
                        echo "Please supply password (leave empty to skip): ";
                        $input = preg_replace('/\r?\n$/', '', `stty -echo; head -n1 ; stty echo`);
                        echo "\n";
                        if ($input != "") {
                            $ssh_pw = $input;
                        } else continue;
                    } else continue;
                } else {
                    $ssh_pw = $file['ssh_pw'];
                }
                $sftp_loc = split(":", $file['source']);
                if (sizeof($sftp_loc) < 2) {
                    if (!$silent) echo "Warning: could not extract host and directory from ".$file['source'].", ssh log:\n";
                    if (!$silent) echo ($sftp->getSFTPLog());
                    if (!$silent) echo "Skipping file!\n";
                    $fail = true;
                    continue;
                }
                if (!$silent) echo "Retreiving files from $sftp_loc[0]...\n";
                $sftp = new Net_SFTP($sftp_loc[0]);
                if (!$sftp->login($file['ssh_user'], $ssh_pw)) {
                    if (!$silent) echo "Warning: could not establish ssh connection with ".$sftp_loc[0].", ssh log:\n";
                    if (!$silent) echo ($sftp->getSFTPLog());
                    if (!$silent) echo "Skipping file!\n";
                    $fail = true;
                    continue;
                }
                $is_dir = $sftp->nlist($sftp_loc[1]);
                //seems we first have to change to root directory, else we're in /home/$sftp_user/
                $sftp->nlist("/");
                if ($is_dir === null) {
                    //single file
                    if (file_exists($file['target']) && !$force_copy) {
                        if (!$silent) echo "Warning: file already exists:". $file['target'] ." - skipping file \n";
                        continue;
                    }
                    if (!$sftp->get($sftp_loc[1], $file['target'])) {
                        if (!$silent) echo "Warning: unable to copy file over ssh: ".$file['source'].", ssh log:\n";
                        if (!$silent) echo ($sftp->getSFTPLog());
                        if (!$silent) echo "Skipping file!\n";
                        $fail = true; 
                    }

                } else {
                    //directory (recursive function)
                    if (!sftp_get_folder($sftp, $sftp_loc[1], $file['target'])) {
                        if (!$silent) echo "Warning: unable to copy file over ssh: ".$file['source'].", ssh log:\n";
                        if (!$silent) echo ($sftp->getSFTPLog());
                        if (!$silent) echo "Skipping file!\n";
                        $fail = true; 
                    }
                }
            } else {
                #local copy
                if (file_exists($file['target']) && !$force_copy) {
                        if (!$silent) echo "File already exists:". $file['target'] ." - skipping file \n";
                        $fail = true;
                        continue;
                }
                #make source location absolute if it looks relative
                if (strpos($file['source'], PATH_SEPARATOR) !== 0) {
                    $file['source'] = $project_maindir.$file['source'];
                }
                exec("cp -R ".$file['source']." ".$file['target'], $cli_ret, $code_ret);

                if ($code_ret != 0) {
                    $fail = true;
                    if (!$silent) echo "Warning: unable to copy file: ".$file['source']." output: \n";
                    foreach ($cli_ret as $line) {
                        echo $line."\n";
                    }
                }
            }
        }                            
        if (!$silent && !$fail) echo "File replacement.....................................................OK\n\n";
        if (!$silent && $fail) echo "File replacement.................................................Not OK\n\n";
    } elseif (!$silent) {          
        echo "File replacement......................................Skipped, no files\n\n";
    }       
} elseif (!$silent) echo "File replacement................................................Skipped\n\n";
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="create_symlinks">
/********************
 * Create Symlinks  *
 *******************/

if(!isset($options['skip-link'])) {
    #iterate all symlinks to create, if there are any
    if (sizeof($symlinks) > 0) {
        $fail = false;
        foreach($symlinks as $link) {
            if (!preg_match("/^\//", $link['source'])) {
                $link['source'] = $project_maindir.$link['source'];
            }
            if (!preg_match("/^\//", $link['target'])) {
                $link['target'] = $project_maindir.$link['target'];
            }
            //make sure target exists
            if (!file_exists($link['target'])) {
                $fail = true;
                if (!$silent) echo "Warning: file ".$link['target']." not found, can't make symlink\n";
                continue;
            }if (file_exists($link['source'])) {
                $fail = true;
                if (!$silent) echo "Warning: file ".$link['source']." already exists, can't make symlink\n";
                continue;   
            }
            $cli_ret = array();
            exec("ln -s ".$link['target']." ".$link['source'], $cli_ret, $code_ret);
            if ($code_ret > 0 && !$silent) {
                $fail = true;
                echo "Warning: unable to create symlink, error:\n";
                foreach ($cli_ret as $line) {
                    echo $line."\n";
                }
            }
        }                            
        if (!$silent && !$fail) echo "Create symlinks......................................................OK\n\n";
        if (!$silent && $fail) echo "Create symlinks..................................................Not OK\n\n";
    } elseif (!$silent) {
             
        echo "Create symlinks.......................................Skipped, no links\n\n";
    }                    
} elseif (!$silent) echo "Create symlinks.................................................Skipped\n\n";
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="yaml_replacements">
/**********************
 * YAML Replacements  *
 *********************/

if(!isset($options['skip-yml'])) {
    $fail = false;
    //first do databases.yml
    if (!file_exists($project_symfonydir."config/databases.yml")) {
        if (!$silent) echo "Warning: ".$project_symfonydir."config/databases.yml not found, cannot change database configuration\n";
        if (!user_continue()) {
            cleanup();
            return 0;
        }
        $fail = true;
    } else {
        $db_config = Spyc::YAMLLoad($project_symfonydir."config/databases.yml");
        if ($db_config['all']['propel']['param'] == null) {
            if (!$silent) echo "Warning: all:propel:param not found in ".$project_symfonydir."config/databases.yml,\n perhaps file is emtpy?\n";
            if (!user_continue()) {
                cleanup();
                return 0;
            }
            $db_config['all']['propel']['class'] = "sfPropelDatabase";
            $db_config['all']['propel']['param']['encoding'] = "utf-8";
        }
        $db_config['all']['propel']['param']['dsn'] = "mysql://$db_user:$db_password@$db_host/$db_name";
        if ($db_config['all']['vbdbdata']['param']['dsn'] == null) {
            $db_config['all']['vbdbdata']['class'] = "sfPropelDatabase";
            $db_config['all']['vbdbdata']['param']['encoding'] = "utf-8";
            $db_config['all']['vbdbdata']['param']['dsn'] = "mysql://$db_user:$db_password@$db_host/$db_name"."_data";
        } else {
            $data_db = $db_config['all']['vbdbdata']['param']['dsn'];
            $data_db_host = substr($data_db, strpos($data_db, "@"));
            $db_config['all']['vbdbdata']['param']['dsn'] = "mysql://$db_user:$db_password$data_db_host";
        }
        $yml_data = Spyc::YAMLDump($db_config, false, 0);
        $yml_fh = fopen($project_symfonydir."config/databases.yml", 'w');
        if (!fwrite($yml_fh, $yml_data)) {
                if (!$silent) echo "Warning: unable to create/write file : ".$project_symfonydir."config/databases.yml\n";
                $fail = true;
        }
        fclose($yml_fh);
    }
    //and for the other yaml replacements from the config
    if (sizeof($yaml_replacements) > 0 ) {
        foreach($yaml_replacements as $yml_rep) {
            if (!file_exists($project_maindir.$yml_rep['file'])) {
                if (!$silent) echo "Warning: YAML replacement file ".$project_maindir.$yml_rep['file']." not found!";
                continue;
            }
            $yml_array = Spyc::YAMLLoad($project_maindir.$yml_rep['file']);
            $yml_key_string = "\$yml_array['".preg_replace('/\:/', "']['", $yml_rep['key'])."']";
            //yes an eval.. definitely the easiest way to get an reference to an element in a multidimensional array with 
            //a variable number of dimensions... following me? ~Sieb
            try {
                eval ('$yml_key_ref = &'.$yml_key_string.';');
            } catch (Exception $e) {
                if (!$silent) echo "Error with YAML replacement for ".$yml_rep['key']." in ".$yml_rep['file']."\n";
                if (!$silent) echo "Exeption: ".$e->getMessage()."\n";
                $fail = true;
                continue;
            }
            $yml_key_ref = $yml_rep['value'];
            $yml_data = Spyc::YAMLDump($yml_array, false, 0);
            $yml_fh = fopen($project_maindir.$yml_rep['file'], 'w');
            if (!fwrite($yml_fh, $yml_data)) {
                if (!$silent) echo "Warning: unable to create/write file : ".$yml_rep['file']."\n";
                $fail = true;
            }

        }
    }
                                 
    if (!$silent && !$fail) echo "YAML replacements....................................................OK\n\n";
    if (!$silent && $fail) echo "YAML replacements................................................Not OK\n\n";
} elseif (!$silent) echo "YAML replacements...............................................skipped\n\n";
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="prepare_database">
/*********************
 * Prepare database  *
 ********************/

if(!isset($options['skip-sql'])) {
    //check for fixture file
    if (!is_file($db_fixture)) {
        if (!$silent) echo "Warning: database fixture file not found: $db_fixture\n";
        $db_fixture = false;
        if (!user_continue()) {
            cleanup();
            return 0;
        }
    }

    //attempt to connect to database server
    $sql = @mysqli_connect($db_host, $db_root_user, $db_root_pw);
    if (!$sql || mysqli_connect_error()) {
        if (!$silent) echo "Unable to connect to mysql on $db_host :".mysqli_connect_error()."\n";
        cleanup();
        return 4; //return code 4 - sql fail
    }

    //check if database already exists
    mysqli_select_db($sql, $db_name);
    $create_db = true;
    $result = $sql->query("SELECT DATABASE()");
    if (!$sql->errno) {
        $row = $result->fetch_row();
        //$result->close();
        if ($row[0] == $db_name) {
            $create_db = false;
            if (!$silent) echo "Warning: database already exists: $db_name\n";
            if ($db_fixture) if (!$silent) echo "If you continue, database will be overwritten by data in $db_fixture.\n";
            if (!user_continue()) {
                cleanup();
                return 0;
            }
        }
    } else {
        if (!$silent) echo "Unable to query if database exists: $db_name\n";
        cleanup();
        return 4; 
    }

    //drop database if needed (that is if there's a fixture file AND an existing database)
    if (!$create_db && $db_fixture) {
        if (!$sql->query("DROP DATABASE ".$db_name)) {
            if (!$silent) echo "Unable to drop existing database $db_name: ".$sql->error."\n";
            cleanup();
            return 4;
        }
        $create_db = true; //after drop set create_db to true so next part will pick it up
    }

    //create database if needed
    if ($create_db) {
        if (!$sql->query("CREATE DATABASE ".$db_name)) {
            if (!$silent) echo "Unable to create database $db_name: ".$sql->error."\n";
            cleanup();
            return 4;
        }
    }
    
    //get the location where the user will login, likely to be localhost (if db is localhost it's also vice versa ofcourse)
    if ($db_host == 'localhost' || $db_host == '127.0.0.1') {
        $user_host = 'localhost';
    } else {
        //user will log into database from a remote location, that should be this location, extract this ip-address
        $cli_ret = array();
        exec('curl http://ipecho.net/plain 2>&1', $cli_ret, $status_ret);
        if ($status_ret != 0 || size_of($cli_ret) < 1 || !preg_match('\d{1,3}(\,\d{1,3}){3}', $cli_ret[0])) {
            //ipecho didn't return ip, lets try ifconfig.me as backup
            $cli_ret = array();
            $user_host = exec('curl http://ifconfig.me 2>&1', $cli_ret, $status_ret);
            if ($status_ret != 0 || size_of($cli_ret) < 1 || !preg_match('\d{1,3}(\,\d{1,3}){3}', $cli_ret[0])) {
                if (!$silent) echo "Warning, unable to get external IP address, curl broken?\n";
                if (!$silent) echo "Database user will be registered from locahost if continued.\n"; 
                if (!user_continue()) {
                    cleanup();
                    return 0;
                }
                $user_host = 'localhost';
            }
        }
        $user_host = $cli_ret[0];
    } 
    
    //check if user exists
    $result = $sql->query("SELECT * FROM mysql.user WHERE user LIKE '$db_user'");
    if ($sql->errno) {
        if (!$silent) echo "Unable to query database $db_name for userlist: ".$sql->error."\n";
        cleanup();
        return 4;     
    }
    $row = $result->fetch_row();
    if (isset($row) && $row != "") {
        if (!$silent) echo "Warning: user $db_user  already exists on mysql server.\n";
        if (!$silent) echo "User password might change when continued.\n";
        if (!user_continue()) {
            cleanup();
            return 0;
        }
        if (!$sql->query("SET PASSWORD FOR '$db_user'@'$user_host' = password('$db_password')")) {
            if (!$silent) echo "Unable to set password for $db_user@$user_host : ".$sql->error."\n";
            cleanup();
            return 4;          
        }
    } else {
        if (!$sql->query("CREATE USER '$db_user'@'$user_host'")) {
            if (!$silent) echo "Unable to create sql user $db_user@$user_host : ".$sql->error."\n";
            cleanup();
            return 4;
        }
    }    
               
    if (!$sql->query("GRANT CREATE,INSERT,DELETE,UPDATE,SELECT ON $db_name.* to '$db_user'@'$user_host'")) {
        if (!$silent) echo "Unable to change privilages for $db_user@$user_host : $sql->error\n";
        cleanup();
        return 4;    
    }
    
    //flush permisssions, (re-)select the database
    $sql->query("FLUSH PRIVILIGES");
    $sql->query("USE ".$db_name);
    
    //load fixture file if needed
    if (file_exists($db_fixture)) {
        $lines = file_get_contents($db_fixture);
        if (!mysqli_multi_query($sql, $lines)) {
            if (!$silent) echo "Unable to import fixture file $db_fixture: ".$sql->error."\n";
            cleanup();
            return 4;
        }
    } elseif (!$silent) echo "Warning: database fixture file $db_fixture not found, skipping";
                       
    if (!$silent) echo "Prepare Database.....................................................OK\n\n";
} elseif (!$silent) echo "Prepare Database................................................Skipped\n\n";
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="apache_config">
/******************
 * Apache config  *
 *****************/

if(!isset($options['skip-apache'])) {
    //check for existing apache file
    $apache_file_remove = false;
    $url_nohttp = preg_replace("/^http:\/\/(.*)/", "$1", $project_url);
    $apache_available_file = $apache_root.'sites-available/'.$url_nohttp;
    $apache_enabled_link = $apache_root.'sites-enabled/'.$url_nohttp;
    if (file_exists($apache_available_file)) {
        if (!$silent) echo "Warning: Apache host-file already exists: $apache_available_file\n";
        if (!$silent) echo "If you continue, that file will be overwritten.\n";
        if (!user_continue()) {
                cleanup();
                return 0;
        }
    } else {
        $apache_file_remove = true;
    }

    //host-file contents
    $apache_contents = '<VirtualHost *:80>
    ServerName '.$project_url.'
    DocumentRoot '.$project_webroot.'
    ScriptAlias /cgi-bin/ '.$project_symfonydir.'cgi-bin/
    DirectoryIndex index.php
    <Directory "'.$project_webroot.'">
        Options FollowSymLinks
        AllowOverride All
        Order allow,deny
        Allow from all
        ExpiresActive On
        ExpiresByType text/css A2592000
        ExpiresByType application/x-javascript A2592000
        ExpiresByType image/gif A2592000
        ExpiresByType image/jpeg A2592000
        ExpiresByType image/png A2592000
        ExpiresByType application/x-shockwave-flash A2592000
        <IfModule mod_deflate.c>
            AddType text/css .css
            #compress content with type html, text, css and js
            AddOutputFilterByType DEFLATE text/html text/plain text/css application/x-javascript
            <IfModule mod_headers.c>
                #properly handle requests coming from behind proxies
                Header append Vary User-Agent
            </IfModule>
            BrowserMatch ^Mozilla/4 gzip-only-text/html
            BrowserMatch ^Mozilla/4\.0[678] no-gzip
            BrowserMatch \bMSIE !no-gzip !gzip-only-text/html
        </IfModule>
    </Directory>
    CustomLog '.$project_logdir.'access.log combined
    ErrorLog '.$project_logdir.'error.log
    </VirtualHost>';

    //write sites-available host-file
    $apache_available_fh = @fopen($apache_available_file, 'w');
    if ($apache_available_fh === false) {
        $clean_apache_temp = true;
        if (!$silent) echo "Warning: unable to open Apache host-file: $apache_available_file\n";
        if (!$silent) echo "Likely a permission issue, creating tempfile and copy with sudo.\n";
        $apache_temp = @fopen('./apache_temp', 'w');
        $bytes = @fwrite($apache_temp, $apache_contents);
        if ($apache_temp === false || $bytes === false) {
            if (!$silent) echo "Unable to create temporary apache host file!\n";
            cleanup();
            return 3;
        }
        fclose($apache_temp);
        $cli_ret = array();
        exec("sudo cp ./apache_temp ".$apache_available_file, $cli_ret, $status_ret);
        if ($status_ret > 0) {
            if (!$silent) echo "Unable to copy temporary apache host file as root!\n";
            cleanup();
            return 3;
        }
        if (!unlink('apache_temp')) {
            if (!$silent) echo "Warning: unable to remove temporary apache host file\n";
        } else {
            $clean_apache_temp = false;
        }
    } else {
        $bytes = @fwrite($apache_available_fh, $apache_contents);
        if ($bytes === false) {
            if (!$silent) echo "Unable to write Apache host-file: $apache_available_file\n";
            cleanup();
            return 3;
        }
        fclose($apache_available_fh);
    }

    //remove symlink if exists
    if (file_exists($apache_enabled_link)) {
        unlink($apache_enabled_link);
    }

    //create symlink for sites-enabled
    if (!symlink($apache_available_file, $apache_enabled_link)) {
        if (!$silent) echo "Warning: unable to create symlink for apache host-file: $apache_available_file\n";
        if (!$silent) echo "Likely a permission issue, trying again with sudo.\n";
        $cli_ret = array();
        exec("sudo ln -s ".$apache_available_file." ".$apache_enabled_link, $cli_ret, $status_ret);
        if ($status_ret > 0) {
            if (!$silent) echo "Unable to create symlink for apache as root!\n";
            cleanup();
            return 3;
        }
    }

    $fail = false;
    if (!$apache_no_restart) {
        $cli_ret = array();
        if (!$silent) echo "Sudo required for restarting Apache\n";
        if ($apache_use_httpd) {
            exec("sudo service httpd restart 2>&1", $cli_ret, $status_ret);
        } else {
            exec("sudo service apache2 restart 2>&1", $cli_ret, $status_ret);
        }
        if ($status_ret != 0) {
            $fail = true;
            if (!$silent) echo "Warning: unable to restart apache, output:\n";
            foreach ($cli_ret as $line) {
                if (!$silent) echo $line."\n";
            }
            if (!user_continue()) {
                cleanup();
                return 0;
            }
        }
    }
                                 
    if (!$silent && !$fail) echo "Apache Config........................................................OK\n\n";
    if (!$silent && $fail) echo "Apache Config....................................Not OK (restart failed)\n\n";
} elseif (!$silent) echo "Apache Config ...................................................Skipped\n\n";
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="cronjobs">
/*************
 * Cronjobs  *
 *************/

if(!isset($options['skip-cronjobs']) && sizeof($cronjobs) > 0) {
    $fail = false;
    if (!$silent) echo "Sudo required for backup and adding lines to crontab\n";
    //backup the crontab
    $cli_ret = array();
    exec('sudo cp /etc/crontab /etc/crontab.bak 2>&1', $cli_ret, $status_ret);
    if ($status_ret != 0) {
        if (!$silent) echo "Unable to backup crontab file, changes can't be undone later\n";
        if (!user_continue()) {
            cleanup();
            return 0;
        }          
    } else {
        $clean_crontab = true;
    }
    foreach ($cronjobs as $job) {
        $timing = $job['timing'];
        if (isset($job['user'])) {
            $user = $job['user'];
        } else {
            $user = 'root';
        }
        if (isset($job['php_file'])) {
            $php_file = $job['php_file'];
        } else {
            $php_file = $project_maindir.'symfony';
        }
        if (isset($job['task'])) {
            $task = $job['task'];
        } else {
            $task = "";
        }
        if (isset($job['output'])) {
            $output = $job['output'];
        } else {
            $output = "/dev/null 2>&1";
        }
        $job_line = "$timing\t$user\t/usr/bin/php\t$php_file $task > $output";
        
        //check wether job isn't added already
        $cli_ret = array();
        exec("grep -F '$job_line' /etc/crontab 2>&1", $cli_ret, $status_ret);
        if (sizeof($cli_ret) > 0) {
            if (!$silent) echo "Warning: exact job for $task already found, skipping...\n";
            $fail = true;
            continue;         
        }
        $cli_ret = array();
        exec("grep $task /etc/crontab 2>&1", $cli_ret, $status_ret);
        if (sizeof($cli_ret) > 0) {
            if (!$silent) echo "Warning: similar job for $task already found.\n";
            if (!$silent) echo "Might add duplicate if continued, type 'no' to skip.\n";
            echo "Continue? y or n? ";
            $input = fgets(STDIN);
            if (!preg_match("/^[Yy]/", $input)) {
                continue;
            }
        }
        
        //add line to crontab
        $cli_ret = array();        
        exec('sudo sh -c "echo \"'.$job_line.'\" >> /etc/crontab 2>&1"', $cli_ret, $status_ret);
        if ($status_ret != 0) {
            if (!$silent) echo "Warning: unable to add line to crontab, output:\n";
            foreach ($cli_ret as $line) {
                if (!$silent) echo $line."\n";
            }
            if (!user_continue()) {
                cleanup();
                return 0;
            }
            $fail = true;
        }
        
    }
                                 
    if (!$silent && $fail) echo "Cronjobs...........................................Not OK, skipped some\n\n";
    if (!$silent && !$fail) echo "Cronjobs.............................................................OK\n\n";
} elseif (!$silent) echo "Cronjobs...........................................................Skipped\n\n";
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="symfony_cache_and_test">
/***************************
 * Symfony cache and test  *
 **************************/
if(!isset($options['skip-symfony'])) {
    /*//etc host file check/update, could help testing. WIP, but not sure if really needed
    $hosts_fh = fopen('/etc/hosts', 'r');
    $hosts_lines = fread($hosts_fh);
    $hosts_url = preg_replace("^(http:////)?www\.", "", $project_url);
    $found = false;
    foreach ($hosts_lines as $line) {
        if (preg_match("127\.0\.0\.1\s+(www\.)?\Q$project_url\E\W", $line)) {
            $found = true;
            break;
        }
    }
    if (!$found) {
        //add $hosts_url to etc/hosts here somehow
    }
    */
    $fail = false;
    $cli_ret = array();
    exec($project_symfonydir."symfony cc", $cli_ret, $code_ret);
    if ($code_ret > 0) {
        $fail = true;
        if (!$silent) echo "Warning, unable to initialize symfony cache\n";
        if (!$silent) echo "Continueing with testing, but this will likely fail\n";
    }
    $cli_ret = array();
    if (!$silent) echo "Busy with symfony test units...\n";
    passthru($project_symfonydir."symfony test-all");
    //exec($project_symfonydir."symfony test-all", $cli_ret, $code_ret);
    //if (!$silent) {
    //    foreach($cli_ret as $line) {
    //        echo $line."\n";
    //    }
    //}
                                 
    if (!$silent && !$fail) echo "Symfony cache and test...............................................OK\n\n";
    if (!$silent && $fail) echo "Symfony cache and test..........................Not OK (caching failed)\n\n";
} elseif (!$silent) echo "Symfony cache and test..........................................Skipped\n\n";
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="finish_up">
/**************
 * Finish up  *
 *************/

//finalize the script
$sql->close();     
if (!$silent) echo "Initialization Successful!..................................Goed Bezig!\n";
return 0;

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="functions">
/**************
 * Functions  *
 *************/

//prompt user to continue or not, return true for continue, false to abort initialization
function user_continue() {
    global $ytoall, $ntoall, $silent;
    if ($ytoall) return true;
    if ($ntoall) {
        if (!$silent) echo "Initialization automatically aborted. - Goodbey!\n";
        return false;
    }
    echo "Continue? y or n? ";
    $input = fgets(STDIN);
    if (!preg_match("/^[Yy]/", $input)) {
        echo "Initialization aborted by user. - Goodbey!\n";
        return false;
    }
    echo "\n";
    return true;
}

//clean up stuff if initialization fails
function cleanup() {
    global $silent, $clean_git, $remove_maindir, $apache_file_remove;
    global $project_maindir, $project_logdir, $project_symfonydir, $project_gitdir, $project_webroot;
    global $apache_available_file, $apache_enabled_link, $sql;
    if (!$silent) "Init aborted, now cleaning up\n";
    if (isset($clean_git) && $clean_git) {
        @rmdir($project_logdir);
        exec("rm -rf \"".$project_gitdir.".git\"");
        exec("rm -rf \"".$project_gitdir.".gitignore\"");
        exec("rm -rf \"".$project_gitdir."*\"");
        exec("rm -rf \"".$project_symfonydir);       
    
    }
    if (isset($remove_maindir) && $remove_maindir) {
        if (!$silent) echo "Attempting to remove created directories\n";   
        exec("rm -rf \"".$project_webroot."\"");
        exec("rm -rf \"".$project_maindir."\"*");
        @rmdir($project_maindir);
    }
    if (isset($apache_file_remove) && $apache_file_remove) {
        @unlink($apache_enabled_link);
        exec("rm ".$apache_available_file);
    }
    if (isset($sql) && $sql) {
        $sql->close();
    }
    //put the backup of crontab back when script fails;
    if (isset($clean_crontab) && $clean_crontab) {
        exec('sudo cp /etc/crontab.bak /etc/crontab 2>&1', $cli_ret, $status_ret);
    }
}

//http://www.frostjedi.com/phpbb3/viewtopic.php?f=46&t=12309#p113038
function sftp_get_folder($sftp, $get, $local) {
   global $silent, $force_copy;
   $skip = false;
   if (is_dir($local) && !$force_copy) {
     if (!$silent) echo "Directory $local already present, skipping files\n";
     //return true;
     $skip = true;
   }    
   @mkdir($local);
   if (!$silent) echo "Downloading directory: $get\n";
   $sftp->chdir($get);
   $r_dir = $sftp->nlist($sftp->pwd());
   
   foreach ($r_dir as $r_fn) {
      switch ($r_fn) {
         case '.':
            break;
         case '..':
            break;
         default:
            $sub_dir = $sftp->nlist($r_fn);
            if ($sub_dir) {
               if (!sftp_get_folder($sftp, "$get/$r_fn", "$local/$r_fn")) return false;
               $sftp->chdir($get);   
            } else if (!$skip) {
               if (!($sftp->get($r_fn,"$local/$r_fn"))) return false;
            }   
      }

   } 
   return true;
}

//Unused function that *could* replace the eval part (untested)
function &getMDArrayKey($key, $array) {
    if ($key === null || $key == "") {
        return $array;
    }
    $pos = strpos($key, ":");
    if ($pos !== false) {
        $cur_key = substr($key, 0, $pos);
        $key = substr($key, $pos+1);
    } else {
        $cur_key = $key;
        $key = null;
    }
    if ($array[$cur_key] === null) {
        return null;
    }
    $cur_array = $array[$cur_key];
    return getMDArrayKey($key, $cur_array);
}
// </editor-fold>

?>
