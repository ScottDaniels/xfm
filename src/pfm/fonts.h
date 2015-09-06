/*
All source and documentation in the xfm tree are published with the following open source license:
Contributions to this source repository are assumed published with the same license. 

=================================================================================================
	(c) Copyright 1995-2015 By E. Scott Daniels. All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are
	permitted provided that the following conditions are met:
	
   		1. Redistributions of source code must retain the above copyright notice, this list of
      		conditions and the following disclaimer.
		
   		2. Redistributions in binary form must reproduce the above copyright notice, this list
      		of conditions and the following disclaimer in the documentation and/or other materials
      		provided with the distribution.
	
	THIS SOFTWARE IS PROVIDED BY E. Scott Daniels ``AS IS'' AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL E. Scott Daniels OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
	ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	
	The views and conclusions contained in the software and documentation are those of the
	authors and should not be interpreted as representing official policies, either expressed
	or implied, of E. Scott Daniels.
=================================================================================================
*/

int TimesRoman[] = {
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 24, 33, 41, 50, 50, 83, 78, 33, 	/*  space ! " # $ % & ' */
	33, 33, 50, 56, 25, 33, 25, 28, 50, 50, 	/*  ( ) * + , - . / 0 1 */
	50, 50, 50, 50, 50, 50, 50, 50, 28, 28, 	/*  2 3 4 5 6 7 8 9 : ; */
	56, 56, 56, 44, 92, 72, 67, 67, 72, 61, 	/*  < = > ? @ A B C D E */
	56, 72, 72, 33, 39, 72, 61, 89, 72, 72, 	/*  F G H I J K L M N O */
	56, 72, 67, 56, 61, 72, 72, 94, 72, 72, 	/*  P Q R S T U V W X Y */
	61, 33, 28, 33, 47, 50, 33, 44, 50, 44, 	/*  Z [ \ ] ^ _ ` a b c */
	50, 44, 33, 50, 50, 28, 28, 50, 28, 78, 	/*  d e f g h i j k l m */
	50, 50, 50, 50, 33, 39, 28, 50, 50, 72, 	/*  n o p q r s t u v w */
	50, 50, 44, 48, 20, 48, 0 };			/*  x y z { | } */;
int HelveticaBold[] = {
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 24, 33, 47, 56, 56, 89, 72, 28, 	/*  space ! " # $ % & ' */
	33, 33, 39, 58, 28, 33, 28, 28, 56, 56, 	/*  ( ) * + , - . / 0 1 */
	56, 56, 56, 56, 56, 56, 56, 56, 33, 33, 	/*  2 3 4 5 6 7 8 9 : ; */
	58, 58, 58, 61, 97, 72, 72, 72, 72, 67, 	/*  < = > ? @ A B C D E */
	61, 78, 72, 28, 56, 72, 61, 83, 72, 78, 	/*  F G H I J K L M N O */
	67, 78, 72, 67, 61, 72, 67, 94, 67, 67, 	/*  P Q R S T U V W X Y */
	61, 33, 28, 33, 58, 56, 28, 56, 61, 56, 	/*  Z [ \ ] ^ _ ` a b c */
	61, 56, 33, 61, 61, 28, 28, 56, 28, 89, 	/*  d e f g h i j k l m */
	61, 61, 61, 61, 39, 56, 33, 61, 56, 78, 	/*  n o p q r s t u v w */
	56, 56, 50, 39, 28, 39, 0 };			/*  x y z { | } */;
int TimesBold[] = {
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 24, 33, 55, 50, 50, 100, 83, 33, 	/*  space ! " # $ % & ' */
	33, 33, 50, 57, 25, 33, 25, 28, 50, 50, 	/*  ( ) * + , - . / 0 1 */
	50, 50, 50, 50, 50, 50, 50, 50, 33, 33, 	/*  2 3 4 5 6 7 8 9 : ; */
	57, 57, 57, 50, 93, 72, 67, 72, 72, 67, 	/*  < = > ? @ A B C D E */
	61, 78, 78, 39, 50, 78, 67, 94, 72, 78, 	/*  F G H I J K L M N O */
	61, 78, 72, 56, 67, 72, 72, 100, 72, 72, 	/*  P Q R S T U V W X Y */
	67, 33, 28, 33, 58, 50, 33, 50, 56, 44, 	/*  Z [ \ ] ^ _ ` a b c */
	56, 44, 33, 50, 56, 28, 33, 56, 28, 83, 	/*  d e f g h i j k l m */
	56, 50, 56, 56, 44, 39, 33, 56, 50, 72, 	/*  n o p q r s t u v w */
	50, 50, 44, 39, 22, 39, 0 };			/*  x y z { | } */;
int Helvetica[] = {
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 24, 28, 35, 56, 56, 89, 67, 22, 	/*  space ! " # $ % & ' */
	33, 33, 39, 58, 28, 33, 28, 28, 56, 56, 	/*  ( ) * + , - . / 0 1 */
	56, 56, 56, 56, 56, 56, 56, 56, 28, 28, 	/*  2 3 4 5 6 7 8 9 : ; */
	58, 58, 58, 56, 102, 67, 67, 72, 72, 67, 	/*  < = > ? @ A B C D E */
	61, 78, 72, 28, 50, 67, 56, 83, 72, 78, 	/*  F G H I J K L M N O */
	67, 78, 72, 67, 61, 72, 67, 94, 67, 67, 	/*  P Q R S T U V W X Y */
	61, 28, 28, 28, 47, 56, 22, 56, 56, 50, 	/*  Z [ \ ] ^ _ ` a b c */
	56, 56, 28, 56, 56, 22, 22, 50, 22, 83, 	/*  d e f g h i j k l m */
	56, 56, 56, 56, 33, 50, 28, 56, 50, 72, 	/*  n o p q r s t u v w */
	50, 50, 50, 33, 26, 33, 0 };			/*  x y z { | } */;
int TimesItalic[] = {
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 24, 33, 42, 50, 50, 83, 78, 33, 	/*  space ! " # $ % & ' */
	33, 33, 50, 67, 25, 33, 25, 28, 50, 50, 	/*  ( ) * + , - . / 0 1 */
	50, 50, 50, 50, 50, 50, 50, 50, 33, 33, 	/*  2 3 4 5 6 7 8 9 : ; */
	67, 67, 67, 50, 92, 61, 61, 67, 72, 61, 	/*  < = > ? @ A B C D E */
	61, 72, 72, 33, 44, 67, 56, 83, 67, 72, 	/*  F G H I J K L M N O */
	61, 72, 61, 50, 56, 72, 61, 83, 61, 56, 	/*  P Q R S T U V W X Y */
	56, 39, 28, 39, 42, 50, 33, 50, 50, 44, 	/*  Z [ \ ] ^ _ ` a b c */
	50, 44, 28, 50, 50, 28, 28, 44, 28, 72, 	/*  d e f g h i j k l m */
	50, 50, 50, 50, 39, 39, 28, 50, 44, 67, 	/*  n o p q r s t u v w */
	44, 44, 39, 40, 28, 40, 0 };			/*  x y z { | } */;
int HelveticaItalic[] = {
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
	28, 28, 24, 28, 35, 56, 56, 89, 67, 22, 	/*  space ! " # $ % & ' */
	33, 33, 39, 58, 28, 33, 28, 28, 56, 56, 	/*  ( ) * + , - . / 0 1 */
	56, 56, 56, 56, 56, 56, 56, 56, 28, 28, 	/*  2 3 4 5 6 7 8 9 : ; */
	58, 58, 58, 56, 102, 67, 67, 72, 72, 67, 	/*  < = > ? @ A B C D E */
	61, 78, 72, 28, 50, 67, 56, 83, 72, 78, 	/*  F G H I J K L M N O */
	67, 78, 72, 67, 61, 72, 67, 94, 67, 67, 	/*  P Q R S T U V W X Y */
	61, 28, 28, 28, 47, 56, 22, 56, 56, 50, 	/*  Z [ \ ] ^ _ ` a b c */
	56, 56, 28, 56, 56, 22, 22, 50, 22, 83, 	/*  d e f g h i j k l m */
	56, 56, 56, 56, 33, 50, 28, 56, 50, 72, 	/*  n o p q r s t u v w */
	50, 50, 50, 33, 26, 33, 0 };			/*  x y z { | } */;
