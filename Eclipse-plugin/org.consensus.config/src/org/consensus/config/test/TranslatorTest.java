package org.consensus.config.test;

import org.consensus.config.handlers.Translator;

public class TranslatorTest{
    public static void main(String[] args) {
        Translator t= new Translator();
        
        try {
			t.gen(true,true,true,true,true,true,3,5,"D:\\Lastvoting\\LastVoting.ho","D:\\Lastvoting\\Output","D:\\Lastvoting");
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
}