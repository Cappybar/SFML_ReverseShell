/*
   YARA Rule Set
   Author: yarGen Rule Generator
   Date: 2026-05-13
   Identifier: malware
   Reference: https://github.com/Neo23x0/yarGen
*/

/* Rule Set ----------------------------------------------------------------- */

rule SFML_ReverseShell {
   meta:
      description = "malware - file SFML_ReverseShell.exe"
      author = "yarGen Rule Generator"
      reference = "https://github.com/Neo23x0/yarGen"
      date = "2026-05-13"
      hash1 = "81e7eacc07c37e5336c7603b5df13fa4b4af77eab4dee67734e38cc1318030b3"
   strings:
      $x1 = "C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe" fullword wide
      $s2 = "-ep bypass -nologo -noni" fullword wide
      $s3 = "AppPolicyGetProcessTerminationMethod" fullword ascii
      $s4 = "CreateProcess failed (" fullword ascii
      $s5 = "GetTempPath2W" fullword ascii
      $s6 = "CreatePipe failed for stdin (" fullword ascii
      $s7 = "        <requestedExecutionLevel level='asInvoker' uiAccess='false' />" fullword ascii
      $s8 = "CreatePipe failed for stderr (" fullword ascii
      $s9 = "CreatePipe failed for stdout (" fullword ascii
      $s10 = "AppPolicyGetThreadInitializationType" fullword ascii
      $s11 = "user@sfml-dev.org" fullword ascii
      $s12 = "WriteFile to stdin failed (" fullword ascii
      $s13 = "FlsGetValue2" fullword ascii
      $s14 = " Type Descriptor'" fullword ascii
      $s15 = "HTTPS protocol is not supported by sf::Http" fullword ascii
      $s16 = "www.sfml-dev.org" fullword ascii
      $s17 = "operator<=>" fullword ascii
      $s18 = "Received from server and sent to PowerShell: " fullword ascii
      $s19 = "operator co_await" fullword ascii
      $s20 = ".data$rs" fullword ascii
   condition:
      uint16(0) == 0x5a4d and
      1 of ($x*) and 4 of them
}

