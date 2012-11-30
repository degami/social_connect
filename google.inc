<?php
/**
 * Copyright 2011 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

require_once 'includes/google/Google_Client.php';
require_once 'includes/google/contrib/Google_Oauth2Service.php';

function _google_oauth_get_client(){
	global $google_apiConfig;
	session_start();

	$google_apiConfig['oauth2_client_id'] = variable_get('social_connect_google_oauth2_client_id','');
	$google_apiConfig['oauth2_client_secret'] = variable_get('social_connect_google_oauth2_client_secret','');
	$google_apiConfig['developer_key'] = variable_get('social_connect_google_developer_key','');
	$google_apiConfig['oauth2_redirect_uri'] = url('user',array('absolute'=>true));

	$client = new Google_Client($google_apiConfig);
	$client->setApplicationName("Google UserInfo Application");
	$client->setApprovalPrompt('auto');
	// Visit https://code.google.com/apis/console?api=plus to generate your
	// oauth2_client_id, oauth2_client_secret, and to register your oauth2_redirect_uri.
	// $client->setClientId('insert_your_oauth2_client_id');
	// $client->setClientSecret('insert_your_oauth2_client_secret');
	// $client->setRedirectUri('insert_your_redirect_uri');
	// $client->setDeveloperKey('insert_your_developer_key');

	return $client;
}

function google_oauth_get_login_link(){
	global $google_apiConfig,$user;

	$client=_google_oauth_get_client();

	$oauth2 = new Google_Oauth2Service($client);
	if (isset($_GET['code'])) {
		$client->authenticate($_GET['code']);
		$_SESSION['google_token'] = $client->getAccessToken();
		//$redirect = 'http://' . $_SERVER['HTTP_HOST'] . $_SERVER['PHP_SELF'];
		$redirect = $google_apiConfig['oauth2_redirect_uri'];
		header('Location: ' . filter_var($redirect, FILTER_SANITIZE_URL));
		return;
	}

	if (isset($_SESSION['google_token'])) {
		$client->setAccessToken($_SESSION['google_token']);
	}

	$google_user=null;
	if ($client->getAccessToken()) {
		$google_user = $oauth2->userinfo->get();
		// The access token may have been updated lazily.
		$_SESSION['google_token'] = $client->getAccessToken();
	} else {
		$authUrl = $client->createAuthUrl();
	}

	if(isset($authUrl)) {
		return $authUrl;
	}

	return $google_user;
}

function google_oauth_logout_user(){
	$client=_google_oauth_get_client();
	unset($_SESSION['google_token']);
	$client->revokeToken();
}