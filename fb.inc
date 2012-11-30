<?php
/**
 * Copyright 2011 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

require_once 'includes/fb/facebook.php';

function _fb_oauth_get_client(){
	// Create our Application instance (replace this with your appId and secret).
	$client = new Facebook(array(
	  'appId'  => variable_get('social_connect_fb_appId',''),
	  'secret' => variable_get('social_connect_fb_secret',''),
	));

	return $client;
}

function fb_oauth_get_login_link(){
	global $user;


	$client = _fb_oauth_get_client();

	$fb_user=null;
	// Get User ID
	$fb_user_id = $client->getUser();

	// We may or may not have this data based on whether the user is logged in.
	//
	// If we have a $user id here, it means we know the user is logged into
	// Facebook, but we don't know if the access token is valid. An access
	// token is invalid if the user logged out of Facebook.

	if ($fb_user_id) {
		try {
			// Proceed knowing you have a logged in user who's authenticated.
			$fb_user = $client->api('/me');
		} catch (FacebookApiException $e) {
			error_log($e);
			$fb_user = $fb_user_id = null;
		}
	} else {
		$loginparams=array(
						'scope'=>'email',
						'redirect_url' => url('user',array('absolute'=>true)),
					);
		if(variable_get('social_connect_in_popup', false) == true){
			$loginparams['display']='popup';
		}

		$authUrl = $client->getLoginUrl($loginparams);
	}

	if(isset($authUrl)) {
		return $authUrl;
	}

	return $fb_user;

}

function fb_oauth_logout_user(){
	$client=_fb_oauth_get_client();
	$client->destroySession();
}
