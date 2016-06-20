# -*- coding: utf-8 -*-
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.support.ui import Select
from selenium.common.exceptions import NoSuchElementException
from selenium.common.exceptions import NoAlertPresentException
import unittest, time, re

class Install(unittest.TestCase):
    def setUp(self):
        self.driver = webdriver.Remote(
                command_executor='http://pcsmrcka.fit.vutbr.cz:4444/wd/hub',
                desired_capabilities=webdriver.common.desired_capabilities.DesiredCapabilities.FIREFOX
                )
        self.driver.set_window_size(1280,800)
        self.driver.implicitly_wait(30)
        self.base_url = "http://pcsmrcka.fit.vutbr.cz:8123"
        self.verificationErrors = []
        self.accept_next_alert = True
    
    def loginToWordpress(self):
        driver = self.driver
        driver.get(self.base_url + "/xmutna00/")
        driver.find_element_by_link_text("Log in").click()
        driver.find_element_by_id("user_login").clear()
        driver.find_element_by_id("user_login").send_keys("xmutna00")
        driver.find_element_by_id("user_pass").clear()
        driver.find_element_by_id("user_pass").send_keys("gbzn")
        driver.find_element_by_id("wp-submit").click()

    def uninstallPlugin(self):
        driver = self.driver
        driver.find_element_by_xpath("//li[@id='menu-plugins']/a/div[3]").click()
        driver.find_element_by_link_text("Deactivate").click()
        driver.find_element_by_css_selector("#the-events-calendar > td.plugin-title > div.row-actions.visible > span.delete > a.delete").click()
        driver.find_element_by_id("submit").click()

    def installPlugin(self):
        driver = self.driver
        driver.find_element_by_xpath("//li[@id='menu-plugins']/a/div[3]").click()
        driver.find_element_by_css_selector("a.add-new-h2").click()
        driver.find_element_by_name("s").clear()
        driver.find_element_by_name("s").send_keys("event")
        driver.find_element_by_id("search-submit").click()
        driver.find_element_by_link_text("Install Now").click()
        driver.find_element_by_link_text("Activate Plugin").click()
    #-------------------------------------------------------------------------------------------
    #1 test na pridanie eventu 
    def test_1add_event(self):
        driver = self.driver
        self.loginToWordpress()
        self.installPlugin()
        driver.find_element_by_xpath("//li[@id='menu-posts-tribe_events']/a/div[3]").click()
        driver.find_element_by_css_selector("h2 > a.add-new-h2").click()
        driver.find_element_by_id("title-prompt-text").click()
        driver.find_element_by_id("title").clear()
        driver.find_element_by_id("title").send_keys("udalost1")
        driver.find_element_by_id("content-html").click()
        driver.find_element_by_id("content").clear()
        driver.find_element_by_id("content").send_keys("abcde")
        driver.find_element_by_id("EventStartDate").click()
        driver.find_element_by_id("EventStartDate").clear()
        driver.find_element_by_id("EventStartDate").send_keys("2015-06-08")
        driver.find_element_by_xpath("//table[@id='EventInfo']/tbody/tr[2]/td/table/tbody/tr[2]/td[2]").click()
        driver.find_element_by_id("EventEndDate").click()
        driver.find_element_by_id("tribe-event-datepickers").click()
        driver.find_element_by_id("EventEndDate").click()   
        driver.find_element_by_id("EventEndDate").clear()
        driver.find_element_by_id("EventEndDate").send_keys("2015-06-09")
        driver.find_element_by_css_selector("td.tribe_sectionheader").click()
        driver.find_element_by_id("publish").click()

    #2 test na pridanie kategorie udalosti
    def test_2category(self):
        driver = self.driver
        self.loginToWordpress()
        #self.installPlugin()
        driver.find_element_by_xpath("//li[@id='menu-posts-tribe_events']/a/div[3]").click()
        driver.find_element_by_link_text("Event Categories").click()
        driver.find_element_by_id("tag-name").clear()
        driver.find_element_by_id("tag-name").send_keys("testCategory")
        driver.find_element_by_id("submit").click()
        #self.uninstallPlugin()
    
    #4 test na pridanie organizatora udalosti
    def test_3organizer(self):
        driver = self.driver
        self.loginToWordpress()
        driver.find_element_by_xpath("//li[@id='menu-posts-tribe_events']/a/div[3]").click()
        driver.find_element_by_link_text("Organizers").click()
        driver.find_element_by_css_selector("a.add-new-h2").click()
        driver.find_element_by_id("title").clear()
        driver.find_element_by_id("title").send_keys("testOrganizer")
        driver.find_element_by_id("content").clear()
        driver.find_element_by_id("content").send_keys("test")
        driver.find_element_by_id("publish").click()

    #4 test na pridanie miesta udalosti
    def test_4venue(self):
        driver = self.driver
        self.loginToWordpress()
        driver.find_element_by_xpath("//li[@id='menu-posts-tribe_events']/a/div[3]").click()
        driver.find_element_by_link_text("Venues").click()
        driver.find_element_by_css_selector("a.add-new-h2").click()
        driver.find_element_by_id("title").clear()
        driver.find_element_by_id("title").send_keys("testVenue")
        driver.find_element_by_id("content").clear()
        driver.find_element_by_id("content").send_keys("test")
        driver.find_element_by_id("publish").click()

    #5 test na editovanie udalosti
    def test_5edit_event(self):
        driver = self.driver
        self.loginToWordpress()
        driver.find_element_by_xpath("//li[@id='menu-posts-tribe_events']/a/div[3]").click()
        driver.find_element_by_link_text("udalost1").click()
        driver.find_element_by_id("content-html").click()
        driver.find_element_by_id("content").clear()
        driver.find_element_by_id("content").send_keys("abcdef")
        driver.find_element_by_id("publish").click()
        driver.find_element_by_css_selector("div.wp-menu-name").click()

    #6  test na zmazanie eventu
    def test_6delete_event(self):
        driver = self.driver
        self.loginToWordpress()
        driver.find_element_by_xpath("//li[@id='menu-posts-tribe_events']/a/div[3]").click()
        driver.find_element_by_link_text("udalost1").click()
        driver.find_element_by_link_text("Move to Trash").click()
        driver.find_element_by_link_text("Trash (1)").click()
        driver.find_element_by_id("delete_all").click()

    #7  test na odinstalovanie pluginu
    def test_7uninstall(self):
        driver = self.driver
        self.loginToWordpress()
        driver.find_element_by_xpath("//li[@id='menu-plugins']/a/div[3]").click()
        driver.find_element_by_link_text("Deactivate").click()
        driver.find_element_by_css_selector("#the-events-calendar > td.plugin-title > div.row-actions.visible > span.delete > a.delete").click()
        driver.find_element_by_id("submit").click()
    
    def is_element_present(self, how, what):
        try: self.driver.find_element(by=how, value=what)
        except NoSuchElementException, e: return False
        return True
    
    def is_alert_present(self):
        try: self.driver.switch_to_alert()
        except NoAlertPresentException, e: return False
        return True
    
    def close_alert_and_get_its_text(self):
        try:
            alert = self.driver.switch_to_alert()
            alert_text = alert.text
            if self.accept_next_alert:
                alert.accept()
            else:
                alert.dismiss()
            return alert_text
        finally: self.accept_next_alert = True
    
    def tearDown(self):
        self.driver.quit()
        self.assertEqual([], self.verificationErrors)

if __name__ == "__main__":
    unittest.main()
