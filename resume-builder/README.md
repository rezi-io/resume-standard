<!-- Implementation -->
## How to Implement

The Resume Metadata Standard (RMS) can be implemented using ExifTool, a powerful metadata manipulation tool. Follow these steps to add RMS metadata to your resume PDFs:

### Prerequisites

1. Install [ExifTool](https://exiftool.org/) on your system:
    - **Windows**: Download the standalone executable from the official website
    - **macOS**: `brew install exiftool` or download from the official website
    - **Linux**: `apt install exiftool` (Debian/Ubuntu) or `yum install perl-Image-ExifTool` (Fedora/CentOS)

### Setting Up the Configuration File

1. Copy the `.ExifTool_config` file from the `resume-builder` folder to one of these locations:
    - **Windows**: `C:\Users\<username>\`
    - **macOS/Linux**: `$HOME/` (your home directory)

   Alternatively, you can specify the config file location with the `-config` option when running ExifTool commands.

### Using the RMS with ExifTool

#### Adding Resume Metadata to a PDF

```bash
exiftool -producer="rms_v2.0.1" \
         -rms_schema_details="https://github.com/rezi-io/resume-standard" \
         -rms_contact_fullName="John Doe" \
         -rms_contact_email="john.doe@example.com" \
         -rms_contact_phone="+1 (555) 123-4567" \
         -rms_experience_count=2 \
         -rms_experience_0_company="Tech Company Inc." \
         -rms_experience_0_role="Senior Developer" \
         -rms_experience_0_dateBegin="2020-01" \
         -rms_experience_0_dateEnd="2023-03" \
         -rms_experience_0_description="Led development team for enterprise applications." \
         resume.pdf
```

### Integration with Applications

To implement the RMS in your resume builder or ATS application:

1. Include the ExifTool configuration as part of your application
2. For web applications, you can use ExifTool via command-line execution or with language-specific wrappers:
    - Node.js: [node-exiftool](https://github.com/MikeKovarik/exiftool-vendored.js)
    - Python: [PyExifTool](https://github.com/sylikc/pyexiftool)
    - PHP: [PHPExiftool](https://github.com/alchemy-fr/PHPExiftool)
    - Java: [metadata-extractor](https://github.com/drewnoakes/metadata-extractor)

### Common Issues and Troubleshooting

- **Metadata not showing up**: Ensure you're using the correct configuration file
- **Permission errors**: Make sure ExifTool has write access to the files
- **Fields not recognized**: Verify that your `.ExifTool_config` file is correctly installed
- **PDF compatibility**: Some PDFs may have restrictions on metadata modification; use `-ignoreMinorErrors` flag

For more information on ExifTool usage, refer to the [ExifTool documentation](https://exiftool.org/exiftool_pod.html).