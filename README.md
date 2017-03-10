# tles
Contains various TLEs (Two-Line Elements) computed by me.  I do this by computing
orbital elements using Find_Orb (http://www.projectpluto.com/find_orb.htm),  generating
ephemerides,  and least-square-fitting TLEs to those ephemerides.  (The code for this
last part isn't public yet;  I need to make it less user-abusive first.)

The purpose of this is to be able to identify certain high-flying artificial satellites
over which asteroid hunters routinely stumble : objects moving slowly enough that it
isn't immediately obvious if the object is a slow-moving artsat or a fast-moving
near-earth object.  If it's the latter,  asteroid hunters will be very interested.
If it's the former,  they would like to ignore it as quickly as possible.

These elements are used in conjunction with my 'sat_id' program (though they could
also be used with any software that uses TLEs.)

Note that updates will occur as new artsats are found and new observations are made
of known artsats.
