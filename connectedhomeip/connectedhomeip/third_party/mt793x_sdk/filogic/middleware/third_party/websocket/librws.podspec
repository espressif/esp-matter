Pod::Spec.new do |s|

# Common settings
  s.name         = "librws"
  s.version      = "1.2.3"
  s.summary      = " Tiny, cross platform websocket client C library"
  s.description  = <<-DESC
Tiny, cross platform websocket client C library.
- No additional dependecies, exceprt pthread on unix-like platforms and Win threads on Windows
- Single header library interface librws.h with public methods
- Thread safe
- Send/receive logic in background thread
                      DESC
  s.homepage     = "https://github.com/OlehKulykov/librws"
  s.license      = { :type => 'MIT', :file => 'LICENSE' }
  s.author       = { "Oleh Kulykov" => "info@resident.name" }
  s.source       = { :git => 'https://github.com/OlehKulykov/librws.git', :tag => s.version.to_s }

# Platforms
  s.ios.deployment_target = "7.0"
  s.osx.deployment_target = "10.7"
  s.watchos.deployment_target = '2.0'
  s.tvos.deployment_target = '9.0'

# Build  
  s.source_files = '*.h', 'src/*.{h,c}', 'contrib/objc/*.{h,m}'
  s.public_header_files = '*.h', 'contrib/objc/*.h'
  s.requires_arc = true
  s.libraries = 'pthread', 'stdc++'
end
